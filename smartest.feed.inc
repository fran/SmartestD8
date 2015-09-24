<?php
/**
 *@file
 */
function smartest_feedback_form($form) {
  
  $query_permission = db_select('smartest_cache')
    ->fields('smartest_cache', array('type'))
    ->condition('cookie', 'permission_cookie', '=')
    ->execute()
    ->fetchAssoc();
  $permision_value = 0;
  if ($query_permission['type'] == 'default') {
    $permision_value = 1;
  }

  $form['#attached']['css'] = drupal_get_path('module', 'smartest') . '/styles/feedback.css';
  $form['feedback'] = array(
    '#title' => t('Feedback'),
    '#description' => t('Please, help us to improve SmarTest with your opinion.'),
    '#type' => 'fieldset',
  );
  $form['feedback']['data'] = array(
    '#title' => t('Identification data'),
    '#type' => 'fieldset',
  );
  $form['feedback']['data']['name-line'] = array(
    '#type' => 'fieldset',
  );
  $form['feedback']['data']['name-line']['name'] = array(
    '#title' => t('Name'),
    '#type' => t('textfield'),
  );
  $form['feedback']['data']['organization-line'] = array(
    '#type' => 'fieldset',
  );
  $form['feedback']['data']['organization-line']['organization'] = array(
    '#title' => t('Organization'),
    '#type' => t('textfield'),
  );
  $form['feedback']['comment'] = array(
    '#title' => t('Comment'),
    '#type' => t('textarea'),
  );
  $form['feedback']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  $form['feedback-perm']['profile']['permission'] = array(
    '#type' => 'fieldset',
  );
  $form['feedback-perm']['profile']['permission']['auth'] = array(
    '#type' => 'checkbox',
    '#title' => t('Anonymous permissions'),
    '#description' => t("Please, allow the Smartest's developers to know about its use."),
    '#default_value' => $permision_value,
  );
  $form['feedback-perm']['profile']['permission']['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#submit' => array('smartest_update_permissions_submit'),
  );

  return $form;
}

function smartest_update_permissions_submit($form, &$form_state) {
  $permision_value = 'no';
  if ($form_state['values']['auth'] == 1) {
    $permision_value = 'default';
  }
  db_update('smartest_cache')
    ->fields(array('type' => $permision_value))
    ->condition('cookie', 'permission_cookie', '=')
    ->execute();
}

function smartest_feedback_form_submit($form, &$form_state) {
  if ($form_state['values']['name'] == "" || $form_state['values']['organization'] == "") {
    drupal_set_message(t('Fields "name" and "organization" are required.'), 'error');
  }
  else {
    smartest_mail_send($form_state['values']);
  }
    
}

function smartest_mail($key, &$message, $params) {
  $user = \Drupal::currentUser();
  $options = array(
    'langcode' => $message['language']->language,
  );

  switch ($key) {
    case 'feedback':
      $message['subject'] = t('Feedback e-mail sent from @site-name', array('@site-name' => \Drupal::config('system.site')->get('name')), $options);
      $message['body'][] = t('@name sent you the following message:', array('@name' => $user->name), $options);
      $message['body'][] = \Drupal\Component\Utility\SafeMarkup::checkPlain($params['name'] . ', from ' . $params['organization'] . ':\n ' . $params['comment']);
      break;
  }
}

/**
 * Sends an e-mail.
 *
 * @param array $form_values
 *   An array of values from the contact form fields that were submitted.
 *   There are just two relevant items: $form_values['email'] and
 *   $form_values['message'].
 */
function smartest_mail_send($form_values) {
  $module = 'smartest';
  $key = 'feedback';
  // @todo: This need to be fixed. Cannot be hardcode.
  $to = 'smartest.drupal@gmail.com';

  $params = $form_values;
  $language = language_default();
  $send = TRUE;
  $result = drupal_mail($module, $key, $to, $language, $params, \Drupal::config('system.site')->get('mail'), $send);
  if ($result['result'] == TRUE) {
    drupal_set_message(t('Your message has been sent.'));
  }
  else {
    drupal_set_message(t('There was a problem sending your message and it was not sent.'), 'error');
  }
}

/**
 * Implements hook_mail_alter().
 *
 * This function is not required to send an email using Drupal's mail system.
 *
 * Hook_mail_alter() provides an interface to alter any aspect of email sent by
 * Drupal. You can use this hook to add a common site footer to all outgoing
 * email, add extra header fields, and/or modify the email in anyway. HTML-izing
 * the outgoing email is one possibility.
 */
function smartest_mail_alter(&$message) {
  $options = array(
    'langcode' => $message['language']->language,
  );

  $signature = t("\n--\nMail altered.", array(), $options);
  if (is_array($message['body'])) {
    $message['body'][] = $signature;
  }
  else {
    $message['body'] .= $signature;
  }
}
