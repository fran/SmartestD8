<?php

/**
 * @file
 * Contains \Drupal\smartest\Form\SmartestGitForm.
 */

namespace Drupal\smartest\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class SmartestGitForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'smartest_git_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'smartest') . '/styles/git-menu.css');


    $query_client = db_select('smartest_cache')
      ->fields('smartest_cache', ['criteria'])
      ->condition('cookie', 'client_id', '=')
      ->execute()
      ->fetchAssoc();
    $query_secret = db_select('smartest_cache')
      ->fields('smartest_cache', ['criteria'])
      ->condition('cookie', 'secret_id', '=')
      ->execute()
      ->fetchAssoc();

    $form['git-description'] = [
      '#type' => 'fieldset',
      '#description' => t('Please, fill out this form with the Github information of Drupal modules. This will enable the extraction of commits information. This proccess can take about one hour.'),
    ];

    $form['git'] = [
      '#type' => 'fieldset',
      '#title' => t('Git commits information'),
      '#description' => t('Please, fill out this form in order to extract the Git changes of your modules.') . '<help>' . \Drupal::l(t('?'), \Drupal\Core\Url::fromUri('https://drive.google.com/open?id=0B-d4IfxRCJFBQ2RYMzlXa0lEYXM')) . '</help>',
    ];
    $form['git']['client-id'] = [
      '#type' => 'textfield',
      '#title' => t('Client ID'),
      '#element_validate' => [
        '_validate_id'
        ],
      '#default_value' => $query_client['criteria'],
    ];
    $form['git']['secret-id'] = [
      '#type' => 'textfield',
      '#title' => t('Secret ID'),
      '#element_validate' => [
        '_validate_id'
        ],
      '#default_value' => $query_secret['criteria'],
    ];
    $form['git']['save-ids'] = [
      '#type' => 'submit',
      '#value' => 'Save IDs',
      '#submit' => [
        'smartest_save_ids'
        ],
    ];
    $form['configuration'] = [
      '#type' => 'fieldset',
      '#title' => t('Cron configuration'),
      '#description' => t('The cron will be run according to the period of time chosen, mining the repository with the Git commits.'),
    ];
    // @FIXME
    // // @FIXME
    // // This looks like another module's variable. You'll need to rewrite this call
    // // to ensure that it uses the correct configuration object.
    // $form['configuration']['cron_interval'] = array(
    //     '#type' => 'select',
    //     '#default_value' => variable_get('cron_example_interval', 60 * 60 * 24),
    //     '#options' => array(
    //       3600 * 12 => t('12 hour'),
    //       60 * 60 * 24 => t('1 day'),
    //       60 * 60 * 24 * 7 => t('1 week'),
    //     ),
    //   );

    $form['submits'] = [];
    $form['submits']['save'] = [
      '#type' => 'submit',
      '#value' => 'Save configurations',
    ];
    $form['submits']['trigger'] = [
      '#type' => 'submit',
      '#value' => t('Run cron now'),
      '#submit' => [
        'mining_repository'
        ],
    ];
    return $form;
  }

}
