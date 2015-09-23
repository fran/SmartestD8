<?php
namespace Drupal\smartest;

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
