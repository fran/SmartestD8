<?php

/**
 * @file
 * Contains \Drupal\smartest\Form\SmartestSettingsForm.
 */

namespace Drupal\smartest\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

class SmartestSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'smartest_settings_form';
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('smartest.settings');

    foreach (Element::children($form) as $variable) {
      $config->set($variable, $form_state->getValue($form[$variable]['#parents']));
    }
    $config->save();

    if (method_exists($this, '_submitForm')) {
      $this->_submitForm($form, $form_state);
    }

    parent::submitForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['smartest.settings'];
  }

  public function buildForm(array $form, FormStateInterface $form_state) {

    $form['general'] = [
      '#type' => 'fieldset',
      '#title' => t('General'),
    ];
    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $form['general']['simpletest_clear_results'] = array(
    //     '#type' => 'checkbox',
    //     '#title' => t('Clear results after each complete test suite run'),
    //     '#description' => t('By default SimpleTest will clear the results after they have been viewed on the results page, but in some cases it may be useful to leave the results in the database. The results can then be viewed at <em>admin/config/development/testing/[test_id]</em>. The test ID can be found in the database, simpletest table, or kept track of when viewing the results the first time. Additionally, some modules may provide more analysis or features that require this setting to be disabled.'),
    //     '#default_value' => variable_get('simpletest_clear_results', TRUE),
    //   );

    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $form['general']['simpletest_verbose'] = array(
    //     '#type' => 'checkbox',
    //     '#title' => t('Provide verbose information when running tests'),
    //     '#description' => t('The verbose data will be printed along with the standard assertions and is useful for debugging. The verbose data will be erased between each test suite run. The verbose data output is very detailed and should only be used when debugging.'),
    //     '#default_value' => variable_get('simpletest_verbose', TRUE),
    //   );


    $form['httpauth'] = [
      '#type' => 'fieldset',
      '#title' => t('HTTP authentication'),
      '#description' => t('HTTP auth settings to be used by the SimpleTest browser during testing. Useful when the site requires basic HTTP authentication.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];
    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $form['httpauth']['simpletest_httpauth_method'] = array(
    //     '#type' => 'select',
    //     '#title' => t('Method'),
    //     '#options' => array(
    //       CURLAUTH_BASIC => t('Basic'),
    //       CURLAUTH_DIGEST => t('Digest'),
    //       CURLAUTH_GSSNEGOTIATE => t('GSS negotiate'),
    //       CURLAUTH_NTLM => t('NTLM'),
    //       CURLAUTH_ANY => t('Any'),
    //       CURLAUTH_ANYSAFE => t('Any safe'),
    //     ),
    //     '#default_value' => variable_get('simpletest_httpauth_method', CURLAUTH_BASIC),
    //   );

    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $username = variable_get('simpletest_httpauth_username');

    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $password = variable_get('simpletest_httpauth_password');

    $form['httpauth']['simpletest_httpauth_username'] = [
      '#type' => 'textfield',
      '#title' => t('Username'),
      '#default_value' => $username,
    ];
    if ($username && $password) {
      $form['httpauth']['simpletest_httpauth_username']['#description'] = t('Leave this blank to delete both the existing username and password.');
    }
    $form['httpauth']['simpletest_httpauth_password'] = [
      '#type' => 'password',
      '#title' => t('Password'),
    ];
    if ($password) {
      $form['httpauth']['simpletest_httpauth_password']['#description'] = t('To change the password, enter the new password here.');
    }
    return parent::buildForm($form, $form_state);
  }

}
