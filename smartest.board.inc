<?php
/**
 *@file
 */
function smartest_dashboard_form($form, &$form_state) {
  // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
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

class GraphicalWidget {
  public $modules = array();
  public $title;
  public $type;
  public $criteria;
  public $n_element;
  public $form;

  public $chart;
  function GraphicalWidget($g_title, $g_modules = array(), $g_type = 'column', $g_criteria = 'loc', $g_n_element = 5) {
    $this->title = $g_title;
    $this->modules = $g_modules;
    $this->type = $g_type;
    $this->criteria = $g_criteria;
    $this->n_element = $g_n_element;
    $this->chart = array(
      '#type' => 'chart',
      '#chart_type' => $this->type,
      '#title' => t('Default'),
      '#legend_position' => 'top',
      '#data_labels' => TRUE,
      '#width' => 500,
      '#height' => 200,
    );

    $id = $this->title;
    $this->form[$id] =  array(
      '#type' => 'fieldset',
      //'#title' => t(identify_criteria($this->criteria) ),
      '#attributes' => array('class' => array('widget')),
    );
    $this->form[$id]['remove ' . $id] = array(
      '#type' => 'image_button',
      '#src' => drupal_get_path('module', 'smartest') . '/imgs/close.png',
      '#prefix' => "<div class='remove-widget'>",
      '#sufix' => '</div>',
      '#name' => $id,
      '#submit' => array('smartest_close_widget_form_submit'),
    );
  }
  function generate_grahical_widget() {
    $id = $this->title;
    if ($this->criteria == 'test') {
      $this->form[$id]['#description'] = $this->generate_test_results_graph();
    }
    else {
      $this->form[$id]['#description'] = $this->generate_stat_grahp();
    }
    return $this->form;
  }

  function generate_stat_grahp() {
    $xaxis_oreder = array();
    $datas = array();
    $query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module', $this->criteria));
    if (!in_array('ALL', $this->modules)) {
      $query->condition('module', $this->modules, 'IN');
    }
    $aux = $query->orderBy($this->criteria, 'DESC')
      ->execute();
    $count = 1;
    while ($row = $aux->fetchAssoc()) {
      $xaxis_oreder[] = $row['module'];
      $datas[] = (int) $row[$this->criteria];
      $count++;
      if ($count > $this->n_element) {
        break;
      }
    }
    $this->chart['xaxis'] = array(
      '#type' => 'chart_xaxis',
      '#labels' => $xaxis_oreder,   
    );
    $this->chart['data'] = array(
      '#type' => 'chart_data',
      '#title' => t($this->criteria),
      '#data' => $datas,
      '#suffix' => '',
    );
    $this->chart['#legend_position'] = 'none';
    $this->chart['#title'] = t(str_replace('-', ' ', $this->title));
    if ($this->criteria == 'passes') {
      $this->chart['data']['#color'] = '#098600';
    }
    $table = array();
    $table['header'] = array();
    $rowt[] = array(
      'data' => \Drupal::service("renderer")->render($this->chart),
      'valign' => 'top',
    );
    $table['rows'][] = $rowt;
    $table['empty'] = t('No fields available.');
    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $content = theme('table', $table);

    return $content;
  }

  function generate_test_results_graph() {
    $xaxis_oreder = array();
    $data_fails = array();
    $data_excp = array();
    $query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module', 'fails', 'exceptions'));
    if (!in_array('ALL', $this->modules)) {
      $query->condition('module', $this->modules, 'IN');
    }
    $aux = $query->orderBy('fails', 'DESC')
      ->execute();
    $count = 1;
    while ($row = $aux->fetchAssoc()) {
      $xaxis_oreder[] = $row['module'];
      $data_fails[] = (int) $row['fails'];
      $data_excp[] = (int) $row['exceptions'];
      $count++;
      if ($count > $this->n_element) {
        break;
      }
    }
    $this->chart['xaxis'] = array(
      '#type' => 'chart_xaxis',
      '#labels' => $xaxis_oreder,   
    );
    $this->chart['fails'] = array(
      '#type' => 'chart_data',
      '#title' => t('Failed tests'),
      '#data' => $data_fails,
      '#suffix' => '',
      '#color' => '#B60000',
    );
    $this->chart['exceptions'] = array(
      '#type' => 'chart_data',
      '#title' => t('Exceptions'),
      '#data' => $data_excp,
      '#suffix' => '',
      '#color' => '#EBE700',
    );
    $this->chart['#title'] = t('Last tests executions');

    $table = array();
    $table['header'] = array();
    $rowt[] = array(
      'data' => \Drupal::service("renderer")->render($this->chart),
      'valign' => 'top',
    );
    $table['rows'][] = $rowt;
    $table['empty'] = t('No fields available.');
    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $content = theme('table', $table);

    return $content;
  }
}

class CloudWidget {
  public $title;
  public $size_criteria;
  public $color_criteria;
  public $form;
  public $modules;

  public $max_color;
  public $min_color;
  public $max_size;
  public $min_size;

  function CloudWidget($c_title, $c_size_criteria, 
    $c_color_criteria, $c_modules) {

    $this->size_criteria = $c_size_criteria;
    $this->color_criteria = $c_color_criteria;
    $this->title = $c_title;
    $this->modules = $c_modules;
    $id = $this->title; 
    $this->form[$id] =  array(
      '#type' => 'fieldset',
      '#title' => t('<h-titles>Size: ' . identify_criteria($this->size_criteria) . ' - Color: ' . identify_criteria($this->color_criteria) . '</h-titles>'),
      '#attributes' => array('class' => array('widget')),
    );
    $this->form[$id]['remove ' . $id] = array(
      '#type' => 'image_button',
      '#src' => drupal_get_path('module', 'smartest') . '/imgs/close.png',
      '#prefix' => "<div class='remove-widget'>",
      '#sufix' => '</div>',
      '#name' => $id,
      '#submit' => array('smartest_close_widget_form_submit'),
    );

    if ($this->size_criteria == 'test') {
      $this->size_criteria = 'fails';
    }
    if ($this->color_criteria == 'test') {
      $this->color_criteria = 'fails';
    }
    $this->max_color = (int) db_query("SELECT MAX(" . $this->color_criteria . ") as max FROM smartest_statistic")->fetchCol()[0];
    $this->min_color = (int) db_query("SELECT MIN(" . $this->color_criteria . ") as min FROM smartest_statistic")->fetchCol()[0];
    $this->max_size = (int) db_query("SELECT MAX(" . $this->size_criteria . ") as max FROM smartest_statistic")->fetchCol()[0];
    $this->min_size = (int) db_query("SELECT MIN(" . $this->size_criteria . ") as min FROM smartest_statistic")->fetchCol()[0];
  }

  function generateCloud() {
    $result   = '';
    $size_query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module', $this->size_criteria));

    if (!in_array('ALL', $this->modules)) {
      $size_query->condition('module', $this->modules, 'IN');
    }
    $sizeaux = $size_query->execute();
    $color_query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module', $this->color_criteria));
  
    if (!in_array('ALL', $this->modules)) {
      $color_query->condition('module', $this->modules, 'IN');
    }

    $coloraux = $color_query->execute();

  while ($row1 = $sizeaux->fetchAssoc()) {
    $row2 = $coloraux->fetchAssoc();
    $description = identify_criteria($this->size_criteria) . ': ' . $row1[$this->size_criteria] . ' - ' .
    identify_criteria($this->color_criteria) . ': ' . $row2[$this->color_criteria];
    $result = $result . ' ' . $this->get_display_term($row1['module'], $this->get_word_index($this->max_size, $this->min_size, $row1[$this->size_criteria]), 
    $this->get_word_index($this->max_color, $this->min_color, $row2[$this->color_criteria]), $description);
  }

    $this->form[$this->title]['#description'] = '<div class="cloud-container"><div class="letter">' . $result . '</div></div>';
    return $this->form;
  }

  function get_display_term($name, $size_index, $color_index, $description) {
    $result = "<span class='tagclouds-term level" . $size_index . " lvlc" . $color_index . "' title='" . $description . "'>" . $name . "</span>\n";
    return $result; 
  }

  function get_word_index($max, $min, $value) {
    $range = $max;
    $result = 1;
    if ($value >= 0 && $value < $range*0.1) {
      $result = 1;
    }
    if ($value >= $range*0.1 && $value < $range*0.2) {
      $result = 2;
    }
    if ($value >= $range*0.2 && $value < $range*0.3) {
      $result = 3;
    }
    if ($value >= $range*0.3 && $value < $range*0.4) {
      $result = 4;
    }
    if ($value >= $range*0.4 && $value < $range*0.5) {
      $result = 5;
    }
    if ($value >= $range*0.5 && $value < $range*0.6) {
      $result = 6;
    }
    if ($value >= $range*0.6 && $value < $range*0.7) {
      $result = 7;
    }
    if ($value >= $range*0.7 && $value < $range*0.8) {
      $result = 8;
    }
    if ($value >= $range*0.8 && $value < $range*0.9) {
      $result = 9;
    }
    if ($value >= $range*0.9 && $value <= $range*1) {
      $result = 10;
    }
    return $result;
  }
}

class HeaderWidget {
  public $title;
  public $failed_percent;
  public $success_percent;
  public $exception_percent;
  public $coverage = 0;
  public $test_passes = 0;
  public $test_fails = 0;
  public $test_exceptions = 0;
  public $test_total = 0;
  public $time = 0;
  public $fails_modules = array();
  public $n_fails_modules = array(); 
  public $lessCoverage = array();
  public $lessCoverageModule = array();
  function HeaderWidget($h_title) {
    $this->title = $h_title;
    $total_fails = 0;
    $total_pass = 0;
    $total_excep = 0;
    $total_time = 0;
      $total_executed = 0;
      $total_executable = 0;
    $query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module', 'fails', 'passes', 'exceptions', 'test_execution_time', 'executed', 'executable'))
      ->orderBy('fails', 'DESC')
      ->execute();
    while ($row = $query->fetchAssoc()) {
      $this->fails_modules[] = $row['module'];
      $this->n_fails_modules[] = (int) $row['fails'];
      $total_fails = $total_fails + (int) $row['fails'];
      $total_pass = $total_pass + (int) $row['passes'];
      $total_excep = $total_excep + (int) $row['exceptions'];
      $total_time = $total_time + (int) $row['test_execution_time'];
      $total_executed = $total_executed + (int) $row['executed'];
      $total_executable = $total_executable + (int) $row['executable'];
    }
    $this->test_total = $total = $total_fails + $total_excep + $total_pass;
    if ($total_executable == 0) {
      $total_executable = 1;
    }
    if ($total_pass != 0) {
      $this->failed_percent = ($total_fails*100)/$total;
      $this->success_percent = ($total_pass*100)/$total ;
      $this->exception_percent = ($total_excep*100)/$total;
      $this->test_passes = $total_pass;
      $this->test_exceptions = $total_excep;
      $this->test_fails = $total_fails;
      $this->coverage = ($total_executed / $total_executable) * 100;              
    }
    else {
      $this->failed_percent = '0';
      $this->success_percent = '0';
      $this->exception_percent = '0';
      $this->test_passes = '0';
      $this->test_exceptions = '0';
      $this->test_fails = '0';
      $this->coverage = '0';          
    }
    $this->time = get_format_time($total_time);

    $query = db_select('smartest_statistic')
      ->fields('smartest_statistic', array('module', 'coverage'))
      ->orderBy('coverage', 'ASC')
      ->execute();

    while ($row = $query->fetchAssoc()) {
      if ((int) $row['coverage'] != 0) {
        $this->lessCoverage[] = $row['coverage'];
        $this->lessCoverageModule[] = $row['module'];
      }
    }
  }

  function generate_header_widget() {
    // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'smartest') . '/styles/graph-menu.css');

    $widget[$this->title] =  array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('widget', 'header', 'header-container')
      ),
    );
    $widget[$this->title]['title'] =  array(
      '#type' => 'fieldset',
      '#description' => t('<h-titles>System information based on previous executions </h-titles>'),
      '#attributes' => array(
        'class' => array('header', 'header-content', 'coverage')
      ),
    );
    $widget[$this->title]['coverage'] =  array(
      '#type' => 'fieldset',
      '#description' => t('<b>Code coverage: </b><coverage>' . round($this->coverage, 2) . '%</coverage>'),
      '#attributes' => array(
        'class' => array('header', 'header-content', 'coverage')
      ),
    );
    $widget[$this->title]['test-failed'] =  array(
      '#type' => 'fieldset',
      '#description' => t('<b>Failed tests: </b><fail>' . round($this->failed_percent, 2) . '%</fail> <label class="total total-fail">Total: ' . $this->test_fails . '</label>'),
      '#attributes' => array(
        'class' => array('header', 'header-content', 'test-failed')
      ),
    );
    $widget[$this->title]['test-exception'] =  array(
      '#type' => 'fieldset',
      '#description' => t('<b>Tests wiht exceptions: </b><exception>' . round($this->exception_percent, 2) . '%</exception> <label class="total total-exception">Total: ' . $this->test_exceptions . '</label>'),
      '#attributes' => array(
        'class' => array('header', 'header-content', 'test-exception')
      ),
    );
    $widget[$this->title]['test-passes'] =  array(
      '#type' => 'fieldset',
      '#description' => t('<b>Passed tests: </b><pass>' . round($this->success_percent, 2) . '%</pass> <label class="total total-pass">Total: ' . $this->test_passes . '</label>'),
      '#attributes' => array(
        'class' => array('header', 'header-content', 'test-passes')
      ),
    );
    $descriptionCov = '';
    for ($i=0; $i < sizeof($this->lessCoverage); $i++) {
      if ($i >= 5) {
        break;
      }
      if ($i != 0) {
        $descriptionCov .= ", ";
      }
      $descriptionCov .= '<degrade' . $i . '>' . $this->lessCoverageModule[$i] . '(' . $this->lessCoverage[$i] . '%)</degrade' . $i . '>';
    }
    $widget[$this->title]['less-coverage'] =  array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('header', 'header-content', 'less-coverage')
      ),
      '#description' => '<b>Modules with less coverage: </b><br>' . $descriptionCov,
    );
    $description = '';
    for ($i=0; $i < 5; $i++) {
      if ( (int) $this->n_fails_modules[0] != 0) {
        if ( (int) $this->n_fails_modules[$i] != 0) {
          if ($i != 0) {
            $description .= ", ";
          }
          $description .= '<degrade' . $i . '>' . $this->fails_modules[$i] 
            . '(' . $this->n_fails_modules[$i] . ')</degrade' . $i . '>';
        }
      }
      else {
      $description = 'no data available';
      } 
    }
    $widget[$this->title]['modules-failed'] =  array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('header', 'header-content', 'modules-failed')
      ),
      '#description' => '<b>Modules with more failures: </b><br>' . $description,
    );
    $widget[$this->title]['test-time'] =  array(
      '#type' => 'fieldset',
      '#description' => t('<b>Time last test execution: </b>' . ' <label class="total total-time">' . $this->time . '</label>'),
      '#attributes' => array(
        'class' => array('header', 'header-content', 'test-passes')
      ),
    );  
    return $widget;
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