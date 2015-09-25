<?php
/**
 * @file
 */

function smartest_git_form($form) {
  $query_client = db_select('smartest_cache')
      ->fields('smartest_cache', array('criteria'))
      ->condition('cookie', 'client_id', '=')
      ->execute()
      ->fetchAssoc();
  $query_secret = db_select('smartest_cache')
      ->fields('smartest_cache', array('criteria'))
      ->condition('cookie', 'secret_id', '=')
      ->execute()
      ->fetchAssoc();

  $form['#attached']['css'] = drupal_get_path('module', 'smartest') . '/styles/git-menu.css';
  $form['git-description'] = array(
    '#type' => 'fieldset',
    '#description' => t('Please, fill out this form with the Github information of Drupal modules. This will enable the extraction of commits information. This proccess can take about one hour.'),
  ); 
      
  $form['git'] = array(
    '#type' => 'fieldset',
    '#title' => t('Git commits information'),
    '#description' => t('Please, fill out this form in order to extract the Git changes of your modules.') . '<help>' . \Drupal::l(t('?'), \Drupal\Core\Url::fromUri('https://drive.google.com/open?id=0B-d4IfxRCJFBQ2RYMzlXa0lEYXM')) . '</help>',
  );
  $form['git']['client-id'] = array(
      '#type' => 'textfield',
      '#title' => t('Client ID'),
      '#element_validate' => array('_validate_id'),
      '#default_value' => $query_client['criteria'],
  );
  $form['git']['secret-id'] = array(
      '#type' => 'textfield',
      '#title' => t('Secret ID'),
      '#element_validate' => array('_validate_id'),
      '#default_value' => $query_secret['criteria'],
  );
  $form['git']['save-ids'] = array(
      '#type' => 'submit',
      '#value' => 'Save IDs',
      '#submit' => array('smartest_save_ids'),
  );  
  $form['configuration'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cron configuration'),
    '#description' => t('The cron will be run according to the period of time chosen, mining the repository with the Git commits.'),
  );
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

  $form['submits'] = array();
  $form['submits']['save'] = array(
      '#type' => 'submit',
      '#value' => 'Save configurations',
      );
  $form['submits']['trigger'] = array(
    '#type' => 'submit',
    '#value' => t('Run cron now'),
    '#submit' => array('mining_repository'),
  );

  return $form;
}

function smartest_save_ids($form, &$form_state) {
    db_update('smartest_cache')
        ->fields(array('criteria' => $form_state['values']['client-id']))
        ->condition('cookie', 'client_id', '=')
        ->execute();
    db_update('smartest_cache')
        ->fields(array('criteria' => $form_state['values']['secret-id']))
        ->condition('cookie', 'secret_id', '=')
        ->execute();
}
function _validate_id($element, &$form_state, $form) {
      if (!empty($element['#value']) && !is_numeric(parse_size($element['#value']))) {
        form_error($element, \Drupal\Component\Utility\SafeMarkup::checkPlain(t('The "!name" option must be a numeric value.', array('!name' => t($element['title'])))));
      }
}

function smartest_help_git_form($form, &$form_state) {
  $form['#attached']['css'] = drupal_get_path('module', 'smartest') . '/styles/git-menu.css';
  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('IDs example'),
    '#description' => t(''),
  );
  $form['help']['img'] = array(
    '#type' => 'image_button',
    '#title' => t('IDs example'),
    '#description' => t(''),
    '#src' => drupal_get_path('module', 'smartest') . '/imgs/id-example.PNG',
  );

  return $form;
}
