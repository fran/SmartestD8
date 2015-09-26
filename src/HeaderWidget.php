<?php

namespace Drupal\smartest;

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
