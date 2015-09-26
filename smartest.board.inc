<?php
/**
 *@file
 */
function smartest_dashboard_form($form, &$form_state) {
  // @FIXME
  // The Assets API has totally changed. CSS, JavaScript, and libraries are now
  // attached directly to render arrays using the #attached property.
  //
  // @see https://www.drupal.org/node/2169605
  // @see https://www.drupal.org/node/2408597
  // drupal_add_css(drupal_get_path('module', 'smartest') . '/styles/graph-menu.css');

  $form['#attached']['js'] = array(
     drupal_get_path('module', 'smartest') . '/smartest.js',
  );

  $library = libraries_load('gridster');

  $query_page = db_select('smartest_cache')
    ->fields('smartest_cache', array('type'))
    ->condition('cookie', 'dashboard_page', '=')
    ->execute()
    ->fetchAssoc();

  $query_all_pages = db_select('smartest_widget')
    ->fields('smartest_widget', array('dashboard_name'))
    ->execute();
  $pages = array();

  while ($row = $query_all_pages->fetchAssoc()) {
    if (!in_array($row['dashboard_name'], $pages)) {
      $pages[$row['dashboard_name']] = $row['dashboard_name'];
    }
  }

  $form['dashboard'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard'),
  );
  $form['dashboard']['update'] = array(
    '#type' => 'submit',
      '#value' => t('Update statistics'),
      '#attributes' => array('class' => array('update')),
      '#submit' => array('update_statistics'),
  );
  $form['dashboard']['profile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Profile'),
  );
  $form['dashboard']['profile']['select'] = array(
    '#type' => 'select',
    '#options' => $pages,
    '#default_value' => $query_page['type'],
  );
  $form['dashboard']['profile']['load'] = array(
  '#type' => 'submit',
      '#value' => t('Load'),
      '#submit' => array('smartest_load_submit'),
  );
  $form['dashboard']['profile']['new_dashboard'] = array(
    '#type' => 'button',
      '#value' => t('New dashboard'),
      '#ajax' => array(
      'callback' => 'smartest_add_page_callback',
      'wrapper' => 'add-page',
      'effect' => 'fade',
      ),
  );
  $form['dashboard']['profile']['addWidget'] = array(
    '#type' => 'button',
      '#value' => t('Add widget'),
      '#ajax' => array(
      'callback' => 'smartest_add_widget_callback',
      'wrapper' => 'add-widget',
      'effect' => 'fade',
      ),
  );
  // Add a new widget form
  $form['dashboard']['add-widget'] = array(
      '#prefix' => '<div id="add-widget">',
      '#suffix' => '</div>',
  );
  if (!empty($form_state['values']['addWidget'])) {

  $form['add-widget'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Widget'),
  );
  $form['add-widget']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Widget title') . ' <fail>*</fail>',
  );
  $form['add-widget']['widget-type'] = array(
    '#type' => 'select',
    '#title' => t('Select a widget type'),
    '#options' => array(
      'chart' => t('Graphic'),
      'cloud' => t('Cloud'),
    ),
    '#default_value' => 'chart',
    '#ajax' => array(
      'callback' => 'smartest_active_div_callback',
      'wrapper' => 'wrapper-act',
    ),
  );
  $form['add-widget']['wrapper'] = array(
    '#prefix' => '<div id="wrapper-act">',
    '#suffix' => '</div>',
  );
  $form['add-widget']['wrapper']['clouds'] = array(
    '#type' => 'fieldset',
    '#title' => 'Clouds configurations',
    '#access' => $form_state['values']['widget-type'] == 'cloud',
  );
  $form['add-widget']['wrapper']['clouds']['selectcriteria1'] = array(
    '#type' => 'select',
    '#title' => t('Select size criteria'),
    '#options' => array(
      'cc' => t('Ciclomatic complexity'),
      'loc' => t('Lines of code'),
      'fails' => t('Failed tests'),
      'exceptions' => t('Tests with exceptions'),
      'test_execution_time' => t('Execution time'),
      'passes' => t('Passed tests'),
      'coverage' => t('Coverage'),
      'git_changes' => t('Git changes'),
    ),
    '#default_value' => 'loc',
  );
  $form['add-widget']['wrapper']['clouds']['selectcriteria2'] = array(
    '#type' => 'select',
    '#title' => t('Select color criteria'),
    '#options' => array(
      'cc' => t('Ciclomatic complexity'),
      'loc' => t('Lines of code'),
      'fails' => t('Failed tests'),
      'exceptions' => t('Tests with exceptions'),
      'test_execution_time' => t('Execution time'),
      'passes' => t('Passed tests'),
      'coverage' => t('Coverage'),
      'git_changes' => t('Git changes'),
    ),
    '#default_value' => 'cc',
  );
  $form['add-widget']['wrapper']['graphics'] = array(
    '#type' => 'fieldset',
    '#title' => 'Graphics configurations',
  );
  $form['add-widget']['wrapper']['graphics']['type'] = array(
    '#type' => 'select',
    '#title' => t('Select graphics types'),
    '#options' => array(
      'column' => t('Column'),
      'line' => t('Line'),
    ),
  );
  $form['add-widget']['wrapper']['graphics']['selectcriteria'] = array(
    '#type' => 'select',
    '#title' => t('Select criteria'),
    '#options' => array(
      'cc' => t('Ciclomatic complexity'),
      'loc' => t('Lines of code'),
      'test' => t('Test Results'),
      'test_execution_time' => t('Execution time (seconds)'),
      'passes' => t('Passed tests'),
      'coverage' => t('Coverage'),
      'git_changes' => t('Git changes'),
    ),
  );
  $form['add-widget']['selectmodules'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select modules'),
    '#options' => array(
      'ALL' => t('All'),
      'top5' => 'Top 5',
      'top10' => 'Top 10',
    ),
    '#default_value' => array('top5'),
  );
  $emods = module_list();
  $count = 1;
  foreach ($emods as $mod => $value) {
    $form['add-widget']['selectmodules']['#options'][$value] = t($mod);
    $count++;
  }
  $form['add-widget']['submit-form'] = array(
    '#type' => 'submit',
    '#value' => t('Generate'),
    '#submit' => array('smartest_add_widget_form_submit'),
  );
  $form['add-widget']['cancel-submit-form'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );

  }

  $form['dashboard']['add-page'] = array(
    '#prefix' => '<div id="add-page">',
    '#suffix' => '</div>',
  );
  if (!empty($form_state['values']['new_dashboard'])) {
    $form['add-page'] = array(
      '#type' => 'fieldset',
      '#title' => 'Add new dashboard',
    );
    $form['add-page']['title-page'] = array(
      '#type' => 'textfield',
      '#title' => t('Dashboard title'),
    );
    $form['add-page']['submit-form'] = array(
      '#type' => 'submit',
      '#value' => t('+ Add'),
      '#submit' => array('smartest_add_page_form_submit'),
    );
    $form['add-page']['cancel-submit-form'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
    );
  }

  $form['dashboard']['wg'] =  array(
    '#prefix' => '<div class="gridster">',
    '#suffix' => '</div>',
  );

  $actual_value = $form['dashboard']['profile']['select']['#default_value'];
  $wgt_query = db_select('smartest_widget')
    ->fields('smartest_widget', array('title', 'widget_type', 'criteria1', 'criteria2', 'modules', 'n_element', 'type'))
    ->condition('dashboard_name', $actual_value, '=')
    ->orderBy('widget_type', 'DESC')
    ->execute();
  while ($row = $wgt_query->fetchAssoc()) {
    $modules = explode(' ', $row['modules']);
    if ($row['widget_type'] == 'main_header') {
      $widget = new HeaderWidget($row['title']);
      $form['dashboard']['wg']['Widget ' . $row['title']] = $widget->generate_header_widget();
    }
    else {
      if ($row['widget_type'] == 'chart') {
        $widget = new GraphicalWidget($row['title'], $modules, $row['type'], $row['criteria1'], $row['n_element']);
        $form['dashboard']['wg']['Widget ' . $row['title']] = $widget->generate_grahical_widget();
      }
      else {
        $widget = new CloudWidget($row['title'], $row['criteria1'], $row['criteria2'], $modules);
        $form['dashboard']['wg']['Widget' . $row['title']] = $widget->generateCloud();
      }
    }
  }

  return $form;
}

function smartest_active_div_callback($form, &$form_state) {
  if ($form_state['values']['widget-type'] == 'chart') {
    $form['add-widget']['wrapper']['clouds']['#access'] = FALSE;
    $form['add-widget']['wrapper']['graphics']['#access'] = TRUE;
  }
  else {
    $form['add-widget']['wrapper']['graphics']['#access'] = FALSE;
    $form['add-widget']['wrapper']['clouds']['#access'] = TRUE;
  }
  return $form['add-widget']['wrapper'];
}

function smartest_add_widget_callback($form, &$form_state) {
  return $form['add-widget'];
}

function smartest_add_page_callback($form, &$form_state) {
  return $form['add-page'];
}

function smartest_load_submit($form, &$form_state) {
  db_update('smartest_cache')
    ->fields(array('type' => $form_state['values']['select']))
    ->condition('cookie', 'dashboard_page', '=')
    ->execute();
}

function smartest_add_page_form_submit($form, &$form_state) {
   $fields = array(
      'title'    => 'header' . $form_state['values']['title-page'],
      'widget_type' => 'main_header',
      'criteria1'     => 'none',
      'criteria2'     => 'none',
      'modules'     => 'ALL',
      'n_element'     => 5,
      'type' => 'column',
      'dashboard_name'     => $form_state['values']['title-page'],
   );
  db_insert('smartest_widget')
    ->fields($fields)
    ->execute();

  db_update('smartest_cache')
    ->fields(array('type' => $form_state['values']['title-page']))
    ->condition('cookie', 'dashboard_page', '=')
    ->execute();
}

function smartest_add_widget_form_submit($form, &$form_state) {
  if ($form_state['values']['title'] != "") {
    // N_element value
    $aux = $form_state['values']['selectmodules'];
    $number = '110';
    if ($aux['ALL'] == TRUE) {
      $number = '1000';
    }
    else {
      if ($aux['top5'] == TRUE) {
        $number = '5';
      }
      else {
        if ($aux['top10'] == TRUE) {
          $number = '10';
        }
      }
    }
    // Modules array value
    $selected = "";
    foreach ($form_state['values']['selectmodules'] as $select => $value) {
      if ($value !== 0 && $value !== 'top5' && $value !== 'top10') {
        $selected = $selected . " " . $select;
      }
    }
    if ($selected == "") {
      $selected =  'ALL';
    }
    // Criterias values
    $widget_type = $form_state['values']['widget-type'];
    $creteria1 = '';
    $criteria2 = '';
    if ($widget_type == 'cloud') {
      $criteria1 = $form_state['values']['selectcriteria1'];
      $criteria2 = $form_state['values']['selectcriteria2'];
    }
    else {
      $criteria1 = $form_state['values']['selectcriteria'];
      $criteria2 = 'none';
    }

    $fields = array(
      'title' => str_replace(' ', '-', $form_state['values']['title']),
      'widget_type' => $widget_type,
      'criteria1'     => $criteria1,
      'criteria2'     => $criteria2,
      'modules' => $selected,
      'n_element' => (int) $number,
      'type' => $form_state['values']['type'],
      'dashboard_name' => $form_state['values']['select'],
    );

    db_insert('smartest_widget')
      ->fields($fields)
      ->execute();
  }
  else {
    drupal_set_message(t('You must give a title to widget.'), 'error');
  }

}

function smartest_close_widget_form_submit($form, &$form_state) {
  $title = $form_state['clicked_button']['#name'];
  drupal_set_message(\Drupal\Component\Utility\SafeMarkup::checkPlain(t('Widget with name "' . $title . '" was removed.')));
  db_delete('smartest_widget')
    ->condition('title', $title)
    ->execute();
}

function update_statistics() {
  drupal_set_time_limit(60 * 5);
  libraries_load('phploc');
  $modules = module_list();
  $files = array();
  foreach ($modules as $module => $value) {
    $analyser = new Analyser;
    $files = file_scan_directory(drupal_get_path("module", $module), '/.*\.inc$|.*\.module$|.*\.php$|.*\.test$|.*\.install$/');
    $files_root = array();
    foreach ($files as $file => $f) {
      if (strpos($file, $module)) {
        $files_root[$file] = $file;
      }
    }
    $results = $analyser->countFiles($files_root, FALSE);
    $fields = array(
      'loc' => $results['ncloc'],
      'cc' => $results['methodCcnMax'],
      'cloc' => $results['cloc'],
      'lloc' => $results['lloc'],
    );
    db_update('smartest_statistic')
      ->fields($fields)
      ->condition('module', $module, '=')
      ->execute();
  }
  update_coverage();

  // Post-process for submodules
  $current = "";
  $last = "";
  $last_module = "";
  foreach ($modules as $name => $value) {
    $current = drupal_get_path('module', $name);
    if ($current == $last) {
      $query = db_select('smartest_statistic')
        ->fields('smartest_statistic', array('loc', 'cc', 'cloc', 'lloc'))
        ->condition('module', $last_module, '=')
        ->execute()
        ->fetchAssoc();

      db_update('smartest_statistic')
        ->fields(array('loc' => $query['loc']))
        ->expression('loc', 'loc - :lamount', array(':lamount' => $query['loc']))
        ->expression('cc', 'cc - :camount', array(':camount' => $query['cc']))
        ->expression('cloc', 'cloc - :clamount', array(':clamount' => $query['cloc']))
        ->expression('lloc', 'lloc - :llamount', array(':llamount' => $query['lloc']))
        ->condition('module', $name, '=')
        ->execute();
    }
    $last = $current;
    $last_module = $name;
  }
}

function update_coverage() {
  db_update('smartest_statistic')
    ->fields(array('executed' => 0, 'executable' => 0))
    ->execute();
  $query = db_select('code_coverage')
    ->fields('code_coverage', array('path', 'executed', 'executable'))
    ->orderBy('path')
    ->execute();
  while ($row = $query->fetchAssoc()) {
    $modules_array = explode('/', $row['path']);
    if (in_array('modules', $modules_array)) {
      $module = '';
      if ($modules_array[0] == 'modules') {
        $module = $modules_array[1];
      }
      else {
        $module = $modules_array[3];
      }
      $last_data = db_select('smartest_statistic')
        ->fields('smartest_statistic', array('executed', 'executable'))
        ->condition('module', $module, '=')
        ->execute()
        ->fetchAssoc();
      $executed = (int) $row['executed'] + (int) $last_data['executed'];
      $executable = (int) $row['executable'] + (int) $last_data['executable'];
      $coverage = ($executed / $executable) * 100;

      $fields = array(
        'executed' => $executed,
        'executable' => $executable,
        'coverage' => $coverage,
      );
      db_update('smartest_statistic')
        ->fields($fields)
        ->condition('module', $module, '=')
        ->execute();
    }
  }
}

function identify_criteria($criteria) {
  $result = '';
  switch ($criteria) {
    case 'loc':
    $result = 'Lines of code';
    break;
  case 'cc':
    $result = 'Ciclomatic complexity';
    break;
  case 'test':
    $result = 'Last tests executions';
    break;
  case 'fails':
    $result = 'Last tests executions (Fails)';
    break;
  case 'passes':
    $result = 'Last tests executions (Passed)';
    break;
  case 'exceptions':
    $result = 'Last tests executions (Exceptions)';
    break;
  case 'test_execution_time':
    $result = 'Execution time';
    break;
  case 'lloc':
    $result = 'Logical lines of code';
    break;
  case 'cloc':
    $result = 'Comment lines of code';
    break;
  case 'coverage':
    $result = 'Coverage';
    break;
  case 'git_changes':
    $result = 'Git changes';
    break;
  default:
    $result = 'Last tests executions (Passed)';
    break;
  }
  return $result;
}

function get_format_time($seconds) {
  $sec = 0;
  $min = 0;
  $result = '';
  if ($seconds >= 60 ) {
    $min = floor($seconds/60);
    $sec = $seconds%60;
    $result = $min . ' min. ' . $sec . ' sec.';
  }
  else {
    $result = $seconds . 'sec.';
  }
  if ($seconds == 0) {
    $result = 'No data available';
  }
  return $result;
}

function update_stats() {
  drupal_set_time_limit( 60 * 5000 );
  libraries_load('phploc');

  $modules = module_list();
  $files = array();

  foreach ($modules as $module => $value) {
    $analyser = new Analyser;
    $files = file_scan_directory(drupal_get_path("module", $module), '/.*\.inc$|.*\.module$|.*\.php$|.*\.test$|.*\.install$/');
    $files_root = array();
    foreach ($files as $file => $f) {
      $files_root[$file] = $file;
    }

    $results = $analyser->countFiles($files_root, FALSE);

    $query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module'))
      ->condition('module', $module, '=')
      ->execute()
      ->fetchAssoc();

    if ($query == null) {
      $fields = array(
        'module'    => $module,
        'loc' => $results['ncloc'],
        'cc' => $results['methodCcnMax'],
        'cloc' => $results['cloc'],
        'lloc' => $results['lloc'],
       );
      db_insert('smartest_statistic')
        ->fields($fields)
        ->execute();
    }
    else {
      $fields = array(
        'loc' => $results['ncloc'],
        'cc' => $results['methodCcnMax'],
        'cloc' => $results['cloc'],
        'lloc' => $results['lloc'],
       );
      db_update('smartest_statistic')
        ->fields($fields)
        ->condition('module', $module, '=')
        ->execute();
    }
  }
}
