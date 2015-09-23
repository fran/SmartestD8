<?php
/**
 * @file
 */

include('smartest.board.inc');

/**
 * List tests arranged in groups that can be selected and run.
 */
function smartest_test_form($form) {
  $query_prior = db_select('smartest_prioritization')
      ->condition('id', 'prioritization_test', '=')
      ->fields('smartest_prioritization', array('last_variable', 'last_order'))
      ->execute()
      ->fetchAssoc(); 
  $prior = $query_prior['last_variable'];
  $order = $query_prior['last_order'];

  $query_time = db_select('smartest_cache')
      ->condition('cookie', 'timeout', '=')
      ->fields('smartest_cache', array('criteria', 'type'))
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

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('<h-titles>Test Case Prioritization</h-titles>'),
    '#description' => t('Please, select the tests ordering criterion. You can also specify the execution time for the tests. Then, <em>Save configuration</em>.'),
  );
  $form['settings']['table'] = array(
      '#type' => 'fieldset',
    );
  $form['settings']['table']['custom'] = array(
    '#type' => 'fieldset',
    '#title' => t('<h-titles>Prioritization criterion</h-titles>'),
  );
  $form['settings']['table']['custom']['selection'] = array(
    '#type' => 'select',
    '#options' => array(
        'loc' => 'Lines of code (Default)',
        'cc' => 'Ciclomatic complexity',
        'fails' => 'Failed tests',
        'exceptions' => 'Tests with exceptions',
        'coverage' => 'Code coverage',
        'git_changes' => t('Git changes'),
      ),
    '#default_value' => $prior,
  );
  $form['settings']['table']['time'] = array(
    '#type' => 'fieldset',
    '#title' => t('<h-titles>Stop execution after: </h-titles>'),
  );
  $form['settings']['table']['time']['input']= array(
     '#type' => 'textfield',
     '#size' => 10,
     '#default_value' => $time,
  );
  $form['settings']['table']['time']['meassure'] = array(
    '#type' => 'select',
    '#options' => array(
        'm' => t('Minutes'),
        'h' => t('Hours'),
      ),
    '#default_value' => strtolower($meassure[0]),
  );
  $form['settings']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('smartest_prioritization_form_submit'),
  );
  $messages = Messages::getMessages($prior, $order);
  $form['tests'] = array(
    '#type' => 'fieldset',
    '#title' => \Drupal\Component\Utility\SafeMarkup::checkPlain(t($messages[0])),
    '#description' => \Drupal\Component\Utility\SafeMarkup::checkPlain(t($messages[1])),
  ); 
  $form['tests']['table'] = array(
    '#theme' => 'smartest_test_table',
  );

  $enableds = module_list();
  $groups = simpletest_test_get_all();

  $prioritization_object = new Prioritization();
  $order_array = $prioritization_object->get_statistics_prioritization($enableds, $prior, $order);
  foreach ($order_array as $mod => $modval) {
    foreach ($groups as $group => $tests) {
      $info = system_get_info('module', $modval['module']);

      if (in_array(strtolower(str_replace(" ", "_", $group)), $enableds) 
        || strtolower($info['package']) == strtolower($group) 
        || strtolower($info['name']) == strtolower($group)) {

        if ($modval['module'] == strtolower(str_replace(" ", "_", $group)) 
          || strtolower($info['package']) == strtolower($group) 
          || strtolower($info['name']) == strtolower($group)) {

          $form['tests']['table'][$group] = array(
            '#collapsed' => TRUE,
          );
          foreach ($tests as $class => $info) {
            $form['tests']['table'][$group][$class] = array(
              '#type' => 'checkbox',
              '#title' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['name']),
              '#description' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['description']),
            );
          }
        }
      }
    }
  }
  
  // Operation buttons.
  $form['tests']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Run tests'),
  );
  $form['clean'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#title' => t('Clean test environment'),
    '#description' => t('Remove tables with the prefix "simpletest" and temporary directories that are left over from tests that crashed. This is intended for developers when creating tests.'),
  );
  $form['clean']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Clean environment'),
    '#submit' => array('simpletest_clean_environment', 'smartest_clean_database'),
  );

  return $form;
}

/**
 * Provides settings form for LabsIsaTest variables.
 *
 * @ingroup forms
 * @see LabsIsaTest_settings_form_validate()
 */
function smartest_settings_form($form, &$form_state) {

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
  );
  // @FIXME
// // @FIXME
// // This looks like another module's variable. You'll need to rewrite this call
// // to ensure that it uses the correct configuration object.
// $form['general']['simpletest_clear_results'] = array(
//     '#type' => 'checkbox',
//     '#title' => t('Clear results after each complete test suite run'),
//     '#description' => t('By default SimpleTest will clear the results after they have been viewed on the results page, but in some cases it may be useful to leave the results in the database. The results can then be viewed at <em>admin/config/development/testing/[test_id]</em>. The test ID can be found in the database, simpletest table, or kept track of when viewing the results the first time. Additionally, some modules may provide more analysis or features that require this setting to be disabled.'),
//     '#default_value' => variable_get('simpletest_clear_results', TRUE),
//   );

  // @FIXME
// // @FIXME
// // This looks like another module's variable. You'll need to rewrite this call
// // to ensure that it uses the correct configuration object.
// $form['general']['simpletest_verbose'] = array(
//     '#type' => 'checkbox',
//     '#title' => t('Provide verbose information when running tests'),
//     '#description' => t('The verbose data will be printed along with the standard assertions and is useful for debugging. The verbose data will be erased between each test suite run. The verbose data output is very detailed and should only be used when debugging.'),
//     '#default_value' => variable_get('simpletest_verbose', TRUE),
//   );


  $form['httpauth'] = array(
    '#type' => 'fieldset',
    '#title' => t('HTTP authentication'),
    '#description' => t('HTTP auth settings to be used by the SimpleTest browser during testing. Useful when the site requires basic HTTP authentication.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // @FIXME
// // @FIXME
// // This looks like another module's variable. You'll need to rewrite this call
// // to ensure that it uses the correct configuration object.
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
// // @FIXME
// // This looks like another module's variable. You'll need to rewrite this call
// // to ensure that it uses the correct configuration object.
// $username = variable_get('simpletest_httpauth_username');

  // @FIXME
// // @FIXME
// // This looks like another module's variable. You'll need to rewrite this call
// // to ensure that it uses the correct configuration object.
// $password = variable_get('simpletest_httpauth_password');

  $form['httpauth']['simpletest_httpauth_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => $username,
  );
  if ($username && $password) {
    $form['httpauth']['simpletest_httpauth_username']['#description'] = t('Leave this blank to delete both the existing username and password.');
  }
  $form['httpauth']['simpletest_httpauth_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
  );
  if ($password) {
    $form['httpauth']['simpletest_httpauth_password']['#description'] = t('To change the password, enter the new password here.');
  }
  return system_settings_form($form);
}

function smartest_prioritization_form_submit($form, &$form_state) {
  $order = 'DESC';
  $variable = $form_state['values']['selection'];
    $fields = array(
      'id'    => 'prioritization_test',
      'last_variable' => $variable,
      'last_order'     => $order,
    );
    db_update('smartest_prioritization')
      ->fields($fields)
      ->execute();
      
  $timeout_time = $form_state['values']['input'];
  $timeout_meassure = $form_state['values']['meassure'];
  $fields_time = array();
  if ($timeout_time != '') {
    $fields_time = array(
      'criteria' => $timeout_time,
      'type'     => $timeout_meassure,
    );
  }
  else {
    $fields_time = array(
      'criteria' => 'none',
      'type'     => 'none',
    );
  }
  db_update('smartest_cache')
    ->condition('cookie', 'timeout', '=')
    ->fields($fields_time)
    ->execute();

  $auth_query = db_select('smartest_cache')
    ->fields('smartest_cache', array('type'))
    ->condition('cookie', 'auth', '=')
    ->execute()
    ->fetchAssoc();

  if ($auth_query['type'] == 'false') {
    send_criteria_report($variable, $order, $timeout_time, $timeout_meassure);
  }
  
}

/**
 * Run selected tests.
 */
function smartest_test_form_submit($form, &$form_state) {
  smartest_clean_database();
  simpletest_classloader_register();
  // Get list of tests.
  $tests_list = array();
  $prioritization = new Prioritization;
  $modules = array();
  foreach ($form_state['values'] as $class_name => $value) {
    // Since class_exists() will likely trigger an autoload lookup,
    // we do the fast check first.
    if ($value === 1 && class_exists($class_name)) {
      $module_name = $prioritization->find_module_name_by_test_class($class_name);
      $path = drupal_get_path('module', $module_name);
      if ($path != '' && $path != NULL) {
        $modules[] = $module_name;
        drupal_set_message(t('Tests values: @values', array('@values' => var_export($module_name, TRUE))));
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
    drupal_set_message(t('Tests values: @values', array('@values' => var_export($prioritization_list, TRUE))));
    $tests_list = $prioritization->alter_order_test_list($tests_list, $prioritization_list);
  }
  if (count($tests_list) > 0) {
    $test_id = smartest_run_tests($tests_list, 'drupal');
    $form_state['redirect'] = 'admin/config/development/testing-labs/results/' . $test_id;
  }
  else {
    drupal_set_message(t('No test(s) selected.'), 'error');
  }
}

function smartest_result_form($form, &$form_state, $test_id) {
  smartest_clean_database();
  // Make sure there are test results to display and a re-run is not being performed.
  $results = array();
  if (is_numeric($test_id) && !$results = smartest_result_get($test_id)) {
    drupal_set_message(t('No test results to display.'), 'error');
    drupal_goto('admin/config/development/testing-labs');
    return $form;
  }

  // Load all classes and include CSS.
  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'simpletest') . '/simpletest.css');


  // Keep track of which test cases passed or failed.
  $filter = array(
    'pass' => array(),
    'fail' => array(),
  );

  // Summary result fieldset.
  $form['result'] = array(
    '#type' => 'fieldset',
    '#title' => t('Results'),
  );
  $form['result']['summary'] = $summary = array(
    '#theme' => 'simpletest_result_summary',
    '#pass' => 0,
    '#fail' => 0,
    '#exception' => 0,
    '#debug' => 0,
  );

  simpletest_classloader_register();

  // Cycle through each test group.
  $header = array(t('Message'), t('Group'), t('Filename'), t('Line'), t('Function'), array('colspan' => 2, 'data' => t('Status')));
  $form['result']['results'] = array();
  foreach ($results as $group => $assertions) {
    // Create group fieldset with summary information.
    $info = call_user_func(array($group, 'getInfo'));

    $form['result']['results'][$group] = array(
      '#type' => 'fieldset',
      '#title' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['name']),
      '#description' => \Drupal\Component\Utility\SafeMarkup::checkPlain($info['description']),
      '#collapsible' => TRUE,
    );
    $form['result']['results'][$group]['summary'] = $summary;
    $group_summary = &$form['result']['results'][$group]['summary'];

    // Create table of assertions for the group.
    $rows = array();
    foreach ($assertions as $assertion) {
      $row = array();
      $row[] = $assertion->message;
      $row[] = $assertion->message_group;
      $row[] = \Drupal::service("file_system")->basename($assertion->file);
      $row[] = $assertion->line;
      $row[] = $assertion->function;
      //$row[] = simpletest_result_status_image($assertion->status);
      $row[] = \Drupal::moduleHandler()->invoke('simpletest', 'result_status_image', [$assertion->status]);

      $class = 'simpletest-' . $assertion->status;
      if ($assertion->message_group == 'Debug') {
        $class = 'simpletest-debug';
      }
      $rows[] = array('data' => $row, 'class' => array($class));

      $group_summary['#' . $assertion->status]++;
      $form['result']['summary']['#' . $assertion->status]++;
    }
    $form['result']['results'][$group]['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );

    // Set summary information.
    $group_summary['#ok'] = $group_summary['#fail'] + $group_summary['#exception'] == 0;
    $form['result']['results'][$group]['#collapsed'] = $group_summary['#ok'];

    // Store test group (class) as for use in filter.
    $filter[$group_summary['#ok'] ? 'pass' : 'fail'][] = $group;
  }

  // Overal summary status.
  $form['result']['summary']['#ok'] = $form['result']['summary']['#fail'] + $form['result']['summary']['#exception'] == 0;

  // Actions.
  // @FIXME
// url() expects a route name or an external URI.
// $form['#action'] = url('admin/config/development/testing-labs/results/re-run');

  $form['action'] = array(
    '#type' => 'fieldset',
    '#title' => t('Actions'),
    '#attributes' => array('class' => array('container-inline')),
    '#weight' => -11,
  );

  $form['action']['filter'] = array(
    '#type' => 'select',
    '#title' => 'Filter',
    '#options' => array(
      'all' => t('All (@count)', array('@count' => count($filter['pass']) + count($filter['fail']))),
      'pass' => t('Pass (@count)', array('@count' => count($filter['pass']))),
      'fail' => t('Fail (@count)', array('@count' => count($filter['fail']))),
    ),
  );
  $form['action']['filter']['#default_value'] = ($filter['fail'] ? 'fail' : 'all');

  // Categorized test classes for to be used with selected filter value.
  $form['action']['filter_pass'] = array(
    '#type' => 'hidden',
    '#default_value' => implode(',', $filter['pass']),
  );
  $form['action']['filter_fail'] = array(
    '#type' => 'hidden',
    '#default_value' => implode(',', $filter['fail']),
  );

  $form['action']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Run tests'),
  );

  $form['action']['return'] = array(
    '#type' => 'link',
    '#title' => t('Return to list'),
    '#href' => 'admin/config/development/testing-labs/list',
  );

  if (is_numeric($test_id)) {
    simpletest_clean_results_table($test_id);
  }

  return $form;
}

function smartest_result_get($test_id) {
  $results = db_select('simpletest')
    ->fields('simpletest')
    ->condition('test_id', $test_id)
    ->orderBy('test_class')
    ->orderBy('message_id')
    ->execute();

  $test_results = array();
  foreach ($results as $result) {
    if (!isset($test_results[$result->test_class])) {
      $test_results[$result->test_class] = array();
    }
    $test_results[$result->test_class][] = $result;
    $clone = array(
      'test_id' => $result->test_id,
      'message_id' => $result->message_id,
      'test_class' => $result->test_class,
      'status' => $result->status,
      'message' => $result->message,
      'message_group' => $result->message_group,
      'function' => $result->function,
      'line' => $result->line,
      'file' => $result->file,
      );
      db_insert('smartest')
       ->fields($clone)
       ->execute();
  }
  return $test_results;
}

function smartest_clean_database() {
  db_truncate('smartest')->execute();
}

//------------------
//--PRIORITIZATION--
//------------------

class Prioritization {
  /*
  *$modules: List of classes to be tested
  *$variable: String with  the variable to be meassured
  *$order: Order string, 'ASC' or 'DESC'
  */
  function get_statistics_prioritization($modules, $variable, $order) {
    $result = array();

    if ($variable!='none') {
      $query = db_select('smartest_statistic')
        ->fields('smartest_statistic', array('module', $variable))
        ->condition('module', $modules, 'IN')
        ->orderBy($variable, $order)
        ->execute();
    }
    else {
      $query = db_select('smartest_statistic')
        ->fields('smartest_statistic', array('module'))
        ->condition('module', $modules, 'IN')
        ->execute();
    }

    while ($row = $query->fetchAssoc()) {
      $result[] = $row;
    }

    return $result;
  }
  /*
  *$class_name: test class name
  */
  function find_module_name_by_test_class($class_name) {
    $class = new $class_name;
    $module_name = strtolower($class->getInfo()['group']);
    return $module_name;
  }

  function alter_order_test_list($tests_list, $order_array) {
    $tests_list_result = array();
    foreach ($order_array as $module_stat => $value) {
      foreach ($tests_list as $test_class => $val) {
        $class = new $val;
        if (strtolower($class->getInfo()['group']) == $value['module']) {
          $tests_list_result[] = $val;
        }
      }
    }
    return $tests_list_result;
  }
}

function send_criteria_report($criteria, $order, $timeout_time, $timeout_meassure) {
  global $user, $base_root;
  $query = db_select('smartest_cache')
    ->fields('smartest_cache', array('type'))
    ->condition('cookie', 'permission_cookie', '=')
    ->execute()
    ->fetchAssoc();
  if ($query['type'] == 'default') {
    $data = array(
      'criteria' => $criteria,
      'order' => $order,
      'timeout' => $timeout_time,
      'timeout_meassure' => $timeout_meassure,
      'request_uri' => $base_root . request_uri(),
      'ip' => \Drupal::request()->getClientIp(),
      'user' => $user->name,
    );
    $headers = array(
      'content-type' => 'application/json',
    );
    $options = array(
      'method' => 'POST',
      'headers' => $headers,
      'data' => \Drupal\Component\Serialization\Json::encode($data),
    );
    // @FIXME
// drupal_http_request() has been replaced by the Guzzle HTTP client, which is bundled
// with Drupal core.
// 
// 
// @see https://www.drupal.org/node/1862446
// @see http://docs.guzzlephp.org/en/latest
// drupal_http_request('https://labs.isa.us.es:8181/smartest-report/report', $options);

    //drupal_http_request('http://localhost:8080/smartest-report/report', $options);

    db_update('smartest_cache')
      ->fields(array('type' => 'true'))
      ->condition('cookie', 'auth', '=')
      ->execute();
  }
}

/**
 * Returns HTML for a test list generated by simpletest_test_form() into a table.
 *
 * @param $variables
 *   An associative array containing:
 *   - table: A render element representing the table.
 *
 * @ingroup themeable
 */
function theme_smartest_test_table($variables) {
  $table = $variables['table'];

  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'simpletest') . '/simpletest.css');

  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_js(drupal_get_path('module', 'simpletest') . '/simpletest.js');

  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_js('misc/tableselect.js');


  // Create header for test selection table.
  $header = array(
    array('class' => array('select-all')),
    array('data' => t('Test'), 'class' => array('simpletest_test')),
    array('data' => t('Description'), 'class' => array('simpletest_description')),
    array('data' => t('Time last execution'), 'class' => array('smartest_times')),
  );

  // Define the images used to expand/collapse the test groups.
  // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $js = array(
//     'images' => array(
//       theme('image', array('path' => 'misc/menu-collapsed.png', 'width' => 7, 'height' => 7, 'alt' => t('Expand'), 'title' => t('Expand'))) . ' <a href="#" class="simpletest-collapse">(' . t('Expand') . ')</a>',
//       theme('image', array('path' => 'misc/menu-expanded.png', 'width' => 7, 'height' => 7, 'alt' => t('Collapse'), 'title' => t('Collapse'))) . ' <a href="#" class="simpletest-collapse">(' . t('Collapse') . ')</a>',
//     ),
//   );


  $criteria_query = db_select('smartest_prioritization')
    ->fields('smartest_prioritization', array('last_variable'))
    ->condition('id', 'prioritization_test', '=')
    ->execute()
    ->fetchAssoc();
  $criteria = $criteria_query['last_variable'];

  // Cycle through each test group and create a row.
  $rows = array();
  foreach (\Drupal\Core\Render\Element::children($table) as $key) {
    $element = &$table[$key];
    $row = array();

    // Make the class name safe for output on the page by replacing all
    // non-word/decimal characters with a dash (-).
    $test_class = strtolower(trim(preg_replace("/[^\w\d]/", "-", $key)));

    // Select the right "expand"/"collapse" image, depending on whether the
    // category is expanded (at least one test selected) or not.
    $collapsed = !empty($element['#collapsed']);
    $image_index = $collapsed ? 0 : 1;

    // Place-holder for checkboxes to select group of tests.
    $row[] = array('id' => $test_class, 'class' => array('simpletest-select-all'));

    // Expand/collapse image and group title.
    $row[] = array(
      'data' => '<div class="simpletest-image" id="simpletest-test-group-' . $test_class . '"></div>' .
        '<label for="' . $test_class . '-select-all" class="simpletest-group-label">' . $key . '</label>',
      'class' => array('simpletest-group-label'),
    );

    $query_data = db_select('smartest_statistic')
      ->fields('smartest_statistic', array($criteria))
      ->condition('module', strtolower(str_replace("-", "_", $test_class)), '=')
      ->execute()
      ->fetchAssoc();
    if ($criteria != 'test_execution_time') {
      if ($criteria == 'coverage') {
        $row[] = array(
          'data' => identify_criteria($criteria) . ': ' . $query_data[$criteria] . ' %',
          'class' => array('simpletest-group-description'),
        );
      }
      else {
        $row[] = array(
          'data' => identify_criteria($criteria) . ': ' . $query_data[$criteria],
          'class' => array('simpletest-group-description'),
        );
      }
    }
    else {
      $time = get_format_time( (int) $query_data[$criteria]);
      $row[] = array(
        'data' => $time,
        'class' => array('simpletest-group-description'),
      );
    }
    $query_times = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('test_execution_time'))
      ->condition('module', strtolower(str_replace("-", "_", $test_class)), '=')
      ->execute()
      ->fetchAssoc();
    $row[] = array(
      'data' => get_format_time( (int) $query_times['test_execution_time'] ),
      'class' => array('simpletest-group-description'),
    );

    $rows[] = array('data' => $row, 'class' => array('simpletest-group'));

    // Add individual tests to group.
    $current_js = array(
      'testClass' => $test_class . '-test',
      'testNames' => array(),
      'imageDirection' => $image_index,
      'clickActive' => FALSE,
    );

    // Sorting $element by children's #title attribute instead of by class name.
    uasort($element, 'element_sort_by_title');

    // Cycle through each test within the current group.
    foreach (\Drupal\Core\Render\Element::children($element) as $test_name) {
      $test = $element[$test_name];
      $row = array();

      $current_js['testNames'][] = $test['#id'];

      // Store test title and description so that checkbox won't render them.
      $title = $test['#title'];
      $description = $test['#description'];

      $test['#title_display'] = 'invisible';
      unset($test['#description']);

      // Test name is used to determine what tests to run.
      $test['#name'] = $test_name;

      $row[] = array(
        'data' => \Drupal::service("renderer")->render($test),
        'class' => array('simpletest-test-select'),
      );
      $row[] = array(
        'data' => '<label for="' . $test['#id'] . '">' . $title . '</label>',
        'class' => array('simpletest-test-label'),
      );
      $row[] = array(
        'data' => '<div class="description">' . $description . '</div>',
        'class' => array('simpletest-test-description'),
      );

      $rows[] = array(
        'data' => $row, 
        'class' => array($test_class . '-test', ($collapsed ? 'js-hide' : '')));
    }
    $js['simpletest-test-group-' . $test_class] = $current_js;
    unset($table[$key]);
  }

  // Add js array of settings.
  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_js(array('simpleTest' => $js), 'setting');


  if (empty($rows)) {
    return '<strong>' . t('No tests to display.') . '</strong>';
  }
  else {
    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// return theme('table', array(
//       'header' => $header,
//       'rows' => $rows,
//       'attributes' => array(
//         'id' => 'simpletest-form-table'
//       )
//     ));

  }
}

//------------------------
//--------Messages--------
//------------------------
class Messages {
   
  static $LOC_TITLE                 = 'Ordering of tests based on the prioritization criterion selected: Lines of Code';
  static $LOC_DESCRIPTION_ASC       = 'Modules with a lower number of LoC (Lines of Code) will have higher priority to be tested.';
  static $LOC_DESCRIPTION_DESC      = 'Modules with a higher number of LoC (Lines of Code) will have higher priority to be tested.';
  static $CC_TITLE                  = 'Ordering of tests based on the prioritization criterion selected: Ciclomatic Complexity';
  static $CC_DESCRIPTION_ASC        = 'Modules with a lower CC (Cyclomatic Complexity) will have higher priority to be tested.';
  static $CC_DESCRIPTION_DESC       = 'Modules with a higher CC (Cyclomatic Complexity) will have higher priority to be tested.';
  static $FAILS_TITLE               = 'Ordering of tests based on the prioritization criterion selected: Faults Test';
  static $FAILS_DESCRIPTION_ASC     = 'Modules with a lower number of faults found on last tests executions, will have higher priority to be tested.';
  static $FAILS_DESCRIPTION_DESC    = 'Modules with a higher number of faults found on last tests executions, will have higher priority to be tested.';
  static $EXC_TITLE                 = 'Ordering of tests based on the prioritization criterion selected: Tests with Eexceptions';
  static $EXC_DESCRIPTION_ASC       = 'Modules with a lower number of exceptions found on last tests executions, will have higher priority to be tested.';
  static $EXC_DESCRIPTION_DESC      = 'Modules with a higher number of exceptions found on last tests executions, will have higher priority to be tested.';
  static $LASTS_TITLE               = 'Ordering of tests based on the prioritization criterion selected: Lasts Executions';
  static $LASTS_DESCRIPTION_ASC     = 'Modules with older test executions have higher priority to execute their tests.';
  static $LASTS_DESCRIPTION_DESC    = 'Modules with newer executions have higher priority to execute their tests.';
  static $EXC_TIME_TITLE            = 'Ordering of tests based on the prioritization criterion selected: Execution Time';
  static $EXC_TIME_DESCRIPTION_ASC  = 'Modules with a lower time of executions will have higher priority to be tested.';
  static $EXC_TIME_DESCRIPTION_DESC = 'Modules with a higher time of executions will have higher priority to be tested.';
  static $COVERAGE                  = 'Ordering of tests based on the prioritization criterion selected: COVERAGE';
  static $COVERAGE_ASC              = 'Modules with a lower code coverage will have higher priority to be tested.';
  static $COVERAGE_DESC             = 'Modules with a higher code coverage will have higher priority to be tested.';
  static $GIT_TITLE                 = 'Ordering of tests based on the prioritization criterion selected: GIT CHANGES';
  static $GIT_DESCRIPTION_ASC       = 'Modules with lower number of git changes will have higher priority to be tested.';
  static $GIT_DESCRIPTION_DESC      = 'Modules with higher number of git changes will have higher priority to be tested.';

  static function getMessages($prior, $order) {
    $result = array();
    switch ($prior) {
      case 'loc':
        $result[] = self::$LOC_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$LOC_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$LOC_DESCRIPTION_DESC;
        }
        break;

      case 'cc':
      $result[] = self::$CC_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$CC_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$CC_DESCRIPTION_DESC;
        }
        break;
      
      case 'exceptions':
        $result[] = self::$EXC_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$EXC_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$EXC_DESCRIPTION_DESC;
        }
        break;

      case 'fails':
        $result[] = self::$FAILS_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$FAILS_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$FAILS_DESCRIPTION_DESC;
        }
        break;

      case 'last_execution':
        $result[] = self::$LASTS_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$LASTS_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$LASTS_DESCRIPTION_DESC;
        }
        break;

      case 'test_execution_time':
        $result[] = self::$EXC_TIME_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$EXC_TIME_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$EXC_TIME_DESCRIPTION_DESC;
        }
        break;

      case 'coverage':
        $result[] = self::$COVERAGE;
        if ($order == 'ASC') {
          $result[] = self::$COVERAGE_ASC;
        }
        else {
          $result[] = self::$COVERAGE_DESC;
        }
        break;

      case 'git_changes':
        $result[] = self::$GIT_TITLE;
        if ($order == 'ASC') {
          $result[] = self::$GIT_DESCRIPTION_ASC;
        }
        else {
          $result[] = self::$GIT_DESCRIPTION_DESC;
        }
      break;

    }
    return $result;
  }
}
