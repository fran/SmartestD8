<?php
namespace Drupal\smartest;

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
