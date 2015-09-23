<?php

/**
 * @file
 * Contains \Drupal\smartest\Form\SmartestTestForm.
 */

namespace Drupal\smartest\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class SmartestTestForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'smartest_test_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $query_prior = db_select('smartest_prioritization')
      ->condition('id', 'prioritization_test', '=')
      ->fields('smartest_prioritization', ['last_variable', 'last_order'])
      ->execute()
      ->fetchAssoc();
    $prior = $query_prior['last_variable'];
    $order = $query_prior['last_order'];

    $query_time = db_select('smartest_cache')
      ->condition('cookie', 'timeout', '=')
      ->fields('smartest_cache', ['criteria', 'type'])
      ->execute()
      ->fetchAssoc();
    $time = $query_time['criteria'];
    $meassure = $query_time['type'];

    // @FIXME
    // The Assets API has totally changed. CSS, JavaScript, and libraries are now
    // attached directly to render arrays using the #attached property.
    // 
    // 
    // @see https://www.drupal.org/node/2169605
    // @see https://www.drupal.org/node/2408597
    // drupal_add_css(drupal_get_path('module', 'smartest') . '/smartest.css');

    $form['settings'] = [
      '#type' => 'fieldset',
      '#title' => t('<h-titles>Test Case Prioritization</h-titles>'),
      '#description' => t('Please, select the tests ordering criterion. You can also specify the execution time for the tests. Then, <em>Save configuration</em>.'),
    ];
    $form['settings']['table'] = ['#type' => 'fieldset'];
    $form['settings']['table']['custom'] = [
      '#type' => 'fieldset',
      '#title' => t('<h-titles>Prioritization criterion</h-titles>'),
    ];
    $form['settings']['table']['custom']['selection'] = [
      '#type' => 'select',
      '#options' => [
        'loc' => 'Lines of code (Default)',
        'cc' => 'Ciclomatic complexity',
        'fails' => 'Failed tests',
        'exceptions' => 'Tests with exceptions',
        'coverage' => 'Code coverage',
        'git_changes' => t('Git changes'),
      ],
      '#default_value' => $prior,
    ];
    $form['settings']['table']['time'] = [
      '#type' => 'fieldset',
      '#title' => t('<h-titles>Stop execution after: </h-titles>'),
    ];
    $form['settings']['table']['time']['input'] = [
      '#type' => 'textfield',
      '#size' => 10,
      '#default_value' => $time,
    ];
    $form['settings']['table']['time']['meassure'] = [
      '#type' => 'select',
      '#options' => [
        'm' => t('Minutes'),
        'h' => t('Hours'),
      ],
      '#default_value' => strtolower($meassure[0]),
    ];
    $form['settings']['op'] = [
      '#type' => 'submit',
      '#value' => t('Save configuration'),
      '#submit' => [
        'smartest_prioritization_form_submit'
        ],
    ];
    $messages = Messages::getMessages($prior, $order);
    $form['tests'] = [
      '#type' => 'fieldset',
      '#title' => \Drupal\Component\Utility\SafeMarkup::checkPlain(t($messages[0])),
      '#description' => \Drupal\Component\Utility\SafeMarkup::checkPlain(t($messages[1])),
    ];
    $form['tests']['table'] = ['#theme' => 'smartest_test_table'];

    $enableds = module_list();
    $groups = simpletest_test_get_all();

    $prioritization_object = new Prioritization();
    $order_array = $prioritization_object->get_statistics_prioritization($enableds, $prior, $order);
    foreach ($order_array as $mod => $modval) {
      foreach ($groups as $group => $tests) {
        $info = system_get_info('module', $modval['module']);

        if (in_array(strtolower(str_replace(" ", "_", $group)), $enableds) || strtolower($info['package']) == strtolower($group) || strtolower($info['name']) == strtolower($group)) {

          if ($modval['module'] == strtolower(str_replace(" ", "_", $group)) || strtolower($info['package']) == strtolower($group) || strtolower($info['name']) == strtolower($group)) {

            $form['tests']['table'][$group] = ['#collapsed' => TRUE];
            foreach ($tests as $class => $info) {
              $form['tests']['table'][$group][$class] = [
                '#type' => 'checkbox',
                '#title' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['name']),
                '#description' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['description']),
              ];
            }
          }
        }
      }
    }

    // Operation buttons.
    $form['tests']['op'] = [
      '#type' => 'submit',
      '#value' => t('Run tests'),
    ];
    $form['clean'] = [
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#title' => t('Clean test environment'),
      '#description' => t('Remove tables with the prefix "simpletest" and temporary directories that are left over from tests that crashed. This is intended for developers when creating tests.'),
    ];
    $form['clean']['op'] = [
      '#type' => 'submit',
      '#value' => t('Clean environment'),
      '#submit' => [
        'simpletest_clean_environment',
        'smartest_clean_database',
      ],
    ];

    return $form;
  }

  public function submitForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    smartest_clean_database();
    simpletest_classloader_register();
    // Get list of tests.
    $tests_list = [];
    $prioritization = new Prioritization();
    $modules = [];
    foreach ($form_state->getValues() as $class_name => $value) {
      // Since class_exists() will likely trigger an autoload lookup,
    // we do the fast check first.
      if ($value === 1 && class_exists($class_name)) {
        $module_name = $prioritization->find_module_name_by_test_class($class_name);
        $path = drupal_get_path('module', $module_name);
        if ($path != '' && $path != NULL) {
          $modules[] = $module_name;
          drupal_set_message(t('Tests values: @values', [
            '@values' => var_export($module_name, TRUE)
            ]));
        }
        $tests_list[] = $class_name;
      }
    }
    $query = db_query('SELECT l.last_variable, l.last_order, l.id
                    AS items 
                    FROM {smartest_prioritization} l  
                    ORDER BY l.id');

    $order = 'DESC';
    $variable = 'none';

    foreach ($query as $value) {
      $variable = $value->last_variable;
      $order = $value->last_order;
    }

    if ($variable != 'none') {
      $prioritization_list = $prioritization->get_statistics_prioritization($modules, $variable, $order);
      drupal_set_message(t('Tests values: @values', [
        '@values' => var_export($prioritization_list, TRUE)
        ]));
      $tests_list = $prioritization->alter_order_test_list($tests_list, $prioritization_list);
    }
    if (count($tests_list) > 0) {
      $test_id = smartest_run_tests($tests_list, 'drupal');
      $form_state->set(['redirect'], 'admin/config/development/testing-labs/results/' . $test_id);
    }
    else {
      drupal_set_message(t('No test(s) selected.'), 'error');
    }
  }

}
