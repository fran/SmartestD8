<?php
namespace Drupal\smartest;

class SmartestResultForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'smartest_result_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state, $test_id = NULL) {
    smartest_clean_database();
    // Make sure there are test results to display and a re-run is not being performed.
    $results = [];
    if (is_numeric($test_id) && !$results = smartest_result_get($test_id)) {
      drupal_set_message(t('No test results to display.'), 'error');
      drupal_goto('admin/config/development/testing-labs');
      return $form;
    }

    $form['#attached']['css'] = drupal_get_path('module', 'simpletest') . '/simpletest.css';

    // Keep track of which test cases passed or failed.
    $filter = [
      'pass' => [],
      'fail' => [],
    ];

    // Summary result fieldset.
    $form['result'] = [
      '#type' => 'fieldset',
      '#title' => t('Results'),
    ];
    $form['result']['summary'] = $summary = [
      '#theme' => 'simpletest_result_summary',
      '#pass' => 0,
      '#fail' => 0,
      '#exception' => 0,
      '#debug' => 0,
    ];

    simpletest_classloader_register();

    // Cycle through each test group.
    $header = [
      t('Message'),
      t('Group'),
      t('Filename'),
      t('Line'),
      t('Function'),
      [
        'colspan' => 2,
        'data' => t('Status'),
      ],
    ];
    $form['result']['results'] = [];
    foreach ($results as $group => $assertions) {
      // Create group fieldset with summary information.
      $info = call_user_func([
        $group,
        'getInfo',
      ]);

      $form['result']['results'][$group] = [
        '#type' => 'fieldset',
        '#title' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['name']),
        '#description' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['description']),
        '#collapsible' => TRUE,
      ];
      $form['result']['results'][$group]['summary'] = $summary;
      $group_summary = &$form['result']['results'][$group]['summary'];

      // Create table of assertions for the group.
      $rows = [];
      foreach ($assertions as $assertion) {
        $row = [];
        $row[] = $assertion->message;
        $row[] = $assertion->message_group;
        $row[] = \Drupal::service("file_system")->basename($assertion->file);
        $row[] = $assertion->line;
        $row[] = $assertion->function;
        //$row[] = simpletest_result_status_image($assertion->status);
        $row[] = \Drupal::moduleHandler()->invoke('simpletest', 'result_status_image', [
          $assertion->status
          ]);

        $class = 'simpletest-' . $assertion->status;
        if ($assertion->message_group == 'Debug') {
          $class = 'simpletest-debug';
        }
        $rows[] = ['data' => $row, 'class' => [$class]];

        $group_summary['#' . $assertion->status]++;
        $form['result']['summary']['#' . $assertion->status]++;
      }
      $form['result']['results'][$group]['table'] = [
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
      ];

      // Set summary information.
      $group_summary['#ok'] = $group_summary['#fail'] + $group_summary['#exception'] == 0;
      $form['result']['results'][$group]['#collapsed'] = $group_summary['#ok'];

      // Store test group (class) as for use in filter.
      $filter[$group_summary['#ok'] ? 'pass' : 'fail'][] = $group;
    }

    // Overal summary status.
    $form['result']['summary']['#ok'] = $form['result']['summary']['#fail'] + $form['result']['summary']['#exception'] == 0;

    // Set action to re-run
    $form['#action'] = \Drupal::url('smartest.result_form', array('test_id' => 're-run'));

    $form['action'] = [
      '#type' => 'fieldset',
      '#title' => t('Actions'),
      '#attributes' => [
        'class' => [
          'container-inline'
          ]
        ],
      '#weight' => -11,
    ];

    $form['action']['filter'] = [
      '#type' => 'select',
      '#title' => 'Filter',
      '#options' => [
        'all' => t('All (@count)', [
          '@count' => count($filter['pass']) + count($filter['fail'])
          ]),
        'pass' => t('Pass (@count)', ['@count' => count($filter['pass'])]),
        'fail' => t('Fail (@count)', [
          '@count' => count($filter['fail'])
          ]),
      ],
    ];
    $form['action']['filter']['#default_value'] = ($filter['fail'] ? 'fail' : 'all');

    // Categorized test classes for to be used with selected filter value.
    $form['action']['filter_pass'] = [
      '#type' => 'hidden',
      '#default_value' => implode(',', $filter['pass']),
    ];
    $form['action']['filter_fail'] = [
      '#type' => 'hidden',
      '#default_value' => implode(',', $filter['fail']),
    ];

    $form['action']['op'] = [
      '#type' => 'submit',
      '#value' => t('Run tests'),
    ];

    $form['action']['return'] = [
      '#type' => 'link',
      '#title' => t('Return to list'),
      '#href' => 'admin/config/development/testing-labs/list',
    ];

    if (is_numeric($test_id)) {
      simpletest_clean_results_table($test_id);
    }

    return $form;
  }

}
