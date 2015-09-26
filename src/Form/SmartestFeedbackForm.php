<?php

/**
 * @file
 * Contains \Drupal\smartest\Form\SmartestFeedbackForm.
 */

namespace Drupal\smartest\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

class SmartestFeedbackForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'smartest_feedback_form';
  }

  public function buildForm(array $form, FormStateInterface $form_state) {
    $query_permission = db_select('smartest_cache')
      ->fields('smartest_cache', ['type'])
      ->condition('cookie', 'permission_cookie', '=')
      ->execute()
      ->fetchAssoc();
    $permision_value = 0;
    if ($query_permission['type'] == 'default') {
      $permision_value = 1;
    }

    $form['#attached']['css'] = drupal_get_path('module', 'smartest') . '/styles/feedback.css';

    $form['feedback'] = [
      '#title' => t('Feedback'),
      '#description' => t('Please, help us to improve SmarTest with your opinion.'),
      '#type' => 'fieldset',
    ];
    $form['feedback']['data'] = [
      '#title' => t('Identification data'),
      '#type' => 'fieldset',
    ];
    $form['feedback']['data']['name-line'] = ['#type' => 'fieldset'];
    $form['feedback']['data']['name-line']['name'] = [
      '#title' => t('Name'),
      '#type' => t('textfield'),
    ];
    $form['feedback']['data']['organization-line'] = ['#type' => 'fieldset'];
    $form['feedback']['data']['organization-line']['organization'] = [
      '#title' => t('Organization'),
      '#type' => t('textfield'),
    ];
    $form['feedback']['comment'] = [
      '#title' => t('Comment'),
      '#type' => t('textarea'),
    ];
    $form['feedback']['submit'] = [
      '#type' => 'submit',
      '#value' => t('Send'),
    ];
    $form['feedback-perm']['profile']['permission'] = ['#type' => 'fieldset'];
    $form['feedback-perm']['profile']['permission']['auth'] = [
      '#type' => 'checkbox',
      '#title' => t('Anonymous permissions'),
      '#description' => t("Please, allow the Smartest's developers to know about its use."),
      '#default_value' => $permision_value,
    ];
    $form['feedback-perm']['profile']['permission']['update'] = [
      '#type' => 'submit',
      '#value' => t('Update'),
      '#submit' => [
        'smartest_update_permissions_submit'
        ],
    ];

    return $form;
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    if ($form_state->getValue(['name']) == "" || $form_state->getValue(['organization']) == "") {
      drupal_set_message(t('Fields "name" and "organization" are required.'), 'error');
    }
    else {
      smartest_mail_send($form_state->getValues());
    }
  }
}

