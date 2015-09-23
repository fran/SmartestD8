<?php
namespace Drupal\smartest;

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
