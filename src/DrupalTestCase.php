<?php

namespace Drupal\smartest;

/**
 * Base class for Drupal tests.
 *
 * Do not extend this class, use one of the subclasses in this file.
 */
abstract class DrupalTestCase {

  /**
   * The test run ID.
   *
   * @var string
   */
  protected $testId;

  /**
   * The database prefix of this test run.
   *
   * @var string
   */
  protected $databasePrefix = NULL;

  /**
   * The original file directory, before it was changed for testing purposes.
   *
   * @var string
   */
  protected $originalFileDirectory = NULL;

  /**
   * Time limit for the test.
   */
  protected $timeLimit = 500;

  /**
   * Current results of this test case.
   *
   * @var Array
   */
  public $results = array(
    '#pass' => 0,
    '#fail' => 0,
    '#exception' => 0,
    '#debug' => 0,
  );

  /**
   * Assertions thrown in that test case.
   *
   * @var Array
   */
  protected $assertions = array();

  /**
   * This class is skipped when looking for the source of an assertion.
   *
   * When displaying which function an assert comes from, it's not too useful
   * to see "drupalWebTestCase->drupalLogin()', we would like to see the test
   * that called it. So we need to skip the classes defining these helper
   * methods.
   */
  protected $skipClasses = array(__CLASS__ => TRUE);

  /**
   * Flag to indicate whether the test has been set up.
   *
   * The setUp() method isolates the test from the parent Drupal site by
   * creating a random prefix for the database and setting up a clean file
   * storage directory. The tearDown() method then cleans up this test
   * environment. We must ensure that setUp() has been run. Otherwise,
   * tearDown() will act on the parent Drupal site rather than the test
   * environment, destroying live data.
   */
  protected $setup = FALSE;

  protected $setupDatabasePrefix = FALSE;

  protected $setupEnvironment = FALSE;

  /**
   * Constructor for DrupalTestCase.
   *
   * @param $test_id
   *   Tests with the same id are reported together.
   */
  public function __construct($test_id = NULL) {
    $this->testId = $test_id;
  }

  /**
   * Internal helper: stores the assert.
   *
   * @param $status
   *   Can be 'pass', 'fail', 'exception'.
   *   TRUE is a synonym for 'pass', FALSE for 'fail'.
   * @param $message
   *   The message string.
   * @param $group
   *   Which group this assert belongs to.
   * @param $caller
   *   By default, the assert comes from a function whose name starts with
   *   'test'. Instead, you can specify where this assert originates from
   *   by passing in an associative array as $caller. Key 'file' is
   *   the name of the source file, 'line' is the line number and 'function'
   *   is the caller function itself.
   */
  protected function assert($status, $message = '', $group = 'Other', array $caller = NULL) {
    // Convert boolean status to string status.
    if (is_bool($status)) {
      $status = $status ? 'pass' : 'fail';
    }

    // Increment summary result counter.
    $this->results['#' . $status]++;

    // Get the function information about the call to the assertion method.
    if (!$caller) {
      $caller = $this->getAssertionCall();
    }

    // Creation assertion array that can be displayed while tests are running.
    $this->assertions[] = $assertion = array(
      'test_id' => $this->testId,
      'test_class' => get_class($this),
      'status' => $status,
      'message' => $message,
      'message_group' => $group,
      'function' => $caller['function'],
      'line' => $caller['line'],
      'file' => $caller['file'],
    );

    // Store assertion for display after the test has completed.
    self::getDatabaseConnection()
      ->insert('simpletest')
      ->fields($assertion)
      ->execute();

    // We do not use a ternary operator here to allow a breakpoint on
    // test failure.
    return ($status == 'pass');
  }

  /**
   * Returns the database connection to the site running Simpletest.
   *
   * @return DatabaseConnection
   *   The database connection to use for inserting assertions.
   */
  public static function getDatabaseConnection() {
    try {
      $connection = Database::getConnection('default', 'simpletest_original_default');
    }
    catch (DatabaseConnectionNotDefinedException $e) {
      // If the test was not set up, the simpletest_original_default
      // connection does not exist.
      $connection = Database::getConnection('default', 'default');
    }

    return $connection;
  }

  /**
   * Store an assertion from outside the testing context.
   *
   * This is useful for inserting assertions that can only be recorded after
   * the test case has been destroyed, such as PHP fatal errors. The caller
   * information is not automatically gathered since the caller is most likely
   * inserting the assertion on behalf of other code. In all other respects
   * the method behaves just like DrupalTestCase::assert() in terms of storing
   * the assertion.
   *
   * @return
   *   Message ID of the stored assertion.
   *
   * @see DrupalTestCase::assert()
   * @see DrupalTestCase::deleteAssert()
   */
  public static function insertAssert($test_id, $test_class, $status, $message = '', $group = 'Other', array $caller = array()) {
    // Convert boolean status to string status.
    if (is_bool($status)) {
      $status = $status ? 'pass' : 'fail';
    }

    $caller += array(
      'function' => t('Unknown'),
      'line' => 0,
      'file' => t('Unknown'),
    );

    $assertion = array(
      'test_id' => $test_id,
      'test_class' => $test_class,
      'status' => $status,
      'message' => $message,
      'message_group' => $group,
      'function' => $caller['function'],
      'line' => $caller['line'],
      'file' => $caller['file'],
    );

    return self::getDatabaseConnection()
      ->insert('simpletest')
      ->fields($assertion)
      ->execute();
  }

  /**
   * Delete an assertion record by message ID.
   *
   * @param $message_id
   *   Message ID of the assertion to delete.
   * @return
   *   TRUE if the assertion was deleted, FALSE otherwise.
   *
   * @see DrupalTestCase::insertAssert()
   */
  public static function deleteAssert($message_id) {
    return (bool) self::getDatabaseConnection()
      ->delete('simpletest')
      ->condition('message_id', $message_id)
      ->execute();
  }

  /**
   * Cycles through backtrace until the first non-assertion method is found.
   *
   * @return
   *   Array representing the true caller.
   */
  protected function getAssertionCall() {
    $backtrace = debug_backtrace();

    // The first element is the call. The second element is the caller.
    // We skip calls that occurred in one of the methods of our base classes
    // or in an assertion function.
   while (($caller = $backtrace[1]) &&
         ((isset($caller['class']) && isset($this->skipClasses[$caller['class']])) ||
           substr($caller['function'], 0, 6) == 'assert')) {
      // We remove that call.
      array_shift($backtrace);
    }

    return _drupal_get_last_caller($backtrace);
  }

  /**
   * Check to see if a value is not false (not an empty string, 0, NULL, or FALSE).
   *
   * @param $value
   *   The value on which the assertion is to be done.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertTrue($value, $message = '', $group = 'Other') {
    return $this->assert((bool) $value, $message ? $message : t('Value @value is TRUE.', array('@value' => var_export($value, TRUE))), $group);
  }

  /**
   * Check to see if a value is false (an empty string, 0, NULL, or FALSE).
   *
   * @param $value
   *   The value on which the assertion is to be done.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertFalse($value, $message = '', $group = 'Other') {
    return $this->assert(!$value, $message ? $message : t('Value @value is FALSE.', array('@value' => var_export($value, TRUE))), $group);
  }

  /**
   * Check to see if a value is NULL.
   *
   * @param $value
   *   The value on which the assertion is to be done.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertNull($value, $message = '', $group = 'Other') {
    return $this->assert(!isset($value), $message ? $message : t('Value @value is NULL.', array('@value' => var_export($value, TRUE))), $group);
  }

  /**
   * Check to see if a value is not NULL.
   *
   * @param $value
   *   The value on which the assertion is to be done.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertNotNull($value, $message = '', $group = 'Other') {
    return $this->assert(isset($value), $message ? $message : t('Value @value is not NULL.', array('@value' => var_export($value, TRUE))), $group);
  }

  /**
   * Check to see if two values are equal.
   *
   * @param $first
   *   The first value to check.
   * @param $second
   *   The second value to check.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertEqual($first, $second, $message = '', $group = 'Other') {
    return $this->assert($first == $second, $message ? $message : t('Value @first is equal to value @second.', array('@first' => var_export($first, TRUE), '@second' => var_export($second, TRUE))), $group);
  }

  /**
   * Check to see if two values are not equal.
   *
   * @param $first
   *   The first value to check.
   * @param $second
   *   The second value to check.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertNotEqual($first, $second, $message = '', $group = 'Other') {
    return $this->assert($first != $second, $message ? $message : t('Value @first is not equal to value @second.', array('@first' => var_export($first, TRUE), '@second' => var_export($second, TRUE))), $group);
  }

  /**
   * Check to see if two values are identical.
   *
   * @param $first
   *   The first value to check.
   * @param $second
   *   The second value to check.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertIdentical($first, $second, $message = '', $group = 'Other') {
    return $this->assert($first === $second, $message ? $message : t('Value @first is identical to value @second.', array('@first' => var_export($first, TRUE), '@second' => var_export($second, TRUE))), $group);
  }

  /**
   * Check to see if two values are not identical.
   *
   * @param $first
   *   The first value to check.
   * @param $second
   *   The second value to check.
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE if the assertion succeeded, FALSE otherwise.
   */
  protected function assertNotIdentical($first, $second, $message = '', $group = 'Other') {
    return $this->assert($first !== $second, $message ? $message : t('Value @first is not identical to value @second.', array('@first' => var_export($first, TRUE), '@second' => var_export($second, TRUE))), $group);
  }

  /**
   * Fire an assertion that is always positive.
   *
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   TRUE.
   */
  protected function pass($message = NULL, $group = 'Other') {
    return $this->assert(TRUE, $message, $group);
  }

  /**
   * Fire an assertion that is always negative.
   *
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @return
   *   FALSE.
   */
  protected function fail($message = NULL, $group = 'Other') {
    return $this->assert(FALSE, $message, $group);
  }

  /**
   * Fire an error assertion.
   *
   * @param $message
   *   The message to display along with the assertion.
   * @param $group
   *   The type of assertion - examples are "Browser", "PHP".
   * @param $caller
   *   The caller of the error.
   * @return
   *   FALSE.
   */
  protected function error($message = '', $group = 'Other', array $caller = NULL) {
    if ($group == 'User notice') {
      // Since 'User notice' is set by trigger_error() which is used for debug
      // set the message to a status of 'debug'.
      return $this->assert('debug', $message, 'Debug', $caller);
    }

    return $this->assert('exception', $message, $group, $caller);
  }

  /**
   * Logs a verbose message in a text file.
   *
   * The link to the verbose message will be placed in the test results as a
   * passing assertion with the text '[verbose message]'.
   *
   * @param $message
   *   The verbose message to be stored.
   *
   * @see simpletest_verbose()
   */
  protected function verbose($message) {
    if ($id = simpletest_verbose($message)) {
      $class_safe = str_replace('\\', '_', get_class($this));
      $url = file_create_url($this->originalFileDirectory . '/simpletest/verbose/' . $class_safe . '-' . $id . '.html');
      // @FIXME
      // l() expects a Url object, created from a route name or external URI.
      // $this->error(l(t('Verbose message'), $url, array('attributes' => array('target' => '_blank'))), 'User notice');
    }
  }

  /**
   * Run all tests in this class.
   *
   * Regardless of whether $methods are passed or not, only method names
   * starting with "test" are executed.
   *
   * @param $methods
   *   (optional) A list of method names in the test case class to run; e.g.,
   *   array('testFoo', 'testBar'). By default, all methods of the class are
   *   taken into account, but it can be useful to only run a few selected test
   *   methods during debugging.
   */
  public function run(array $methods = array()) {
    module_load_include('inc', 'code_coverage', 'code_coverage.xdebug');
    // Initialize verbose debugging.
    $class = get_class($this);
    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // simpletest_verbose(NULL, variable_get('file_public_path', conf_path() . '/files'), str_replace('\\', '_', $class));


    // HTTP auth settings (<username>:<password>) for the simpletest browser
    // when sending requests to the test site.
    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $this->httpauth_method = variable_get('simpletest_httpauth_method', CURLAUTH_BASIC);

    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $username = variable_get('simpletest_httpauth_username', NULL);

    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $password = variable_get('simpletest_httpauth_password', NULL);

    if ($username && $password) {
      $this->httpauth_credentials = $username . ':' . $password;
    }

    set_error_handler(array($this, 'errorHandler'));
    // Iterate through all the methods in this class, unless a specific list of
    // methods to run was passed.
    $class_methods = get_class_methods($class);
    if ($methods) {
      $class_methods = array_intersect($class_methods, $methods);
    }
    foreach ($class_methods as $method) {
      // If the current method starts with "test", run it - it's a test.
      if (strtolower(substr($method, 0, 4)) == 'test') {
        // Insert a fail record. This will be deleted on completion to ensure
        // that testing completed.
        $method_info = new ReflectionMethod($class, $method);
        $caller = array(
          'file' => $method_info->getFileName(),
          'line' => $method_info->getStartLine(),
          'function' => $class . '->' . $method . '()',
        );

        // Store the caller information in the test_info.
        $GLOBALS['drupal_test_info'] = array(
          'test_id' => $this->testId,
          'test_run_id' => '',
          'in_child_site' => FALSE,
          'caller' => $caller,
        );
        code_coverage_start();

        $completion_check_id = DrupalTestCase::insertAssert($this->testId, $class, FALSE, t('The test did not complete due to a fatal error.'), 'Completion check', $caller);
        $this->setUp();
        if ($this->setup) {
          try {
            $this->$method();
            // Finish up.
          }
          catch (Exception $e) {
            $this->exceptionHandler($e);
          }
          $this->tearDown();
        }
        else {
          $this->fail(t("The test cannot be executed because it has not been set up properly."));
        }
        // Remove the completion check record.
        DrupalTestCase::deleteAssert($completion_check_id);

        unset($GLOBALS['drupal_test_info']);
        code_coverage_stop(FALSE);

      }
    }
    // Clear out the error messages and restore error handler.
    drupal_get_messages();
    restore_error_handler();


    code_coverage_record($this->testId);

  }

  /**
   * Handle errors during test runs.
   *
   * Because this is registered in set_error_handler(), it has to be public.
   * @see set_error_handler
   */
  public function errorHandler($severity, $message, $file = NULL, $line = NULL) {
    if ($severity & error_reporting()) {
      $error_map = array(
        E_STRICT => 'Run-time notice',
        E_WARNING => 'Warning',
        E_NOTICE => 'Notice',
        E_CORE_ERROR => 'Core error',
        E_CORE_WARNING => 'Core warning',
        E_USER_ERROR => 'User error',
        E_USER_WARNING => 'User warning',
        E_USER_NOTICE => 'User notice',
        E_RECOVERABLE_ERROR => 'Recoverable error',
      );

      // PHP 5.3 adds new error logging constants. Add these conditionally for
      // backwards compatibility with PHP 5.2.
      if (defined('E_DEPRECATED')) {
        $error_map += array(
          E_DEPRECATED => 'Deprecated',
          E_USER_DEPRECATED => 'User deprecated',
        );
      }

      $backtrace = debug_backtrace();
      $this->error($message, $error_map[$severity], _drupal_get_last_caller($backtrace));
    }
    return TRUE;
  }

  /**
   * Handle exceptions.
   *
   * @see set_exception_handler
   */
  protected function exceptionHandler($exception) {
    $backtrace = $exception->getTrace();
    // Push on top of the backtrace the call that generated the exception.
    array_unshift($backtrace, array(
      'line' => $exception->getLine(),
      'file' => $exception->getFile(),
    ));
    require_once \Drupal::root() . '/includes/errors.inc';
    // The exception message is run through check_plain() by _drupal_decode_exception().
    $this->error(t('%type: !message in %function (line %line of %file).', _drupal_decode_exception($exception)), 'Uncaught exception', _drupal_get_last_caller($backtrace));
  }

  /**
   * Generates a random string of ASCII characters of codes 32 to 126.
   *
   * The generated string includes alpha-numeric characters and common
   * miscellaneous characters. Use this method when testing general input
   * where the content is not restricted.
   *
   * Do not use this method when special characters are not possible (e.g., in
   * machine or file names that have already been validated); instead,
   * use DrupalWebTestCase::randomName().
   *
   * @param $length
   *   Length of random string to generate.
   *
   * @return
   *   Randomly generated string.
   *
   * @see DrupalWebTestCase::randomName()
   */
  public static function randomString($length = 8) {
    $str = '';
    for ($i = 0; $i < $length; $i++) {
      $str .= chr(mt_rand(32, 126));
    }
    return $str;
  }

  /**
   * Generates a random string containing letters and numbers.
   *
   * The string will always start with a letter. The letters may be upper or
   * lower case. This method is better for restricted inputs that do not
   * accept certain characters. For example, when testing input fields that
   * require machine readable values (i.e. without spaces and non-standard
   * characters) this method is best.
   *
   * Do not use this method when testing unvalidated user input. Instead, use
   * DrupalWebTestCase::randomString().
   *
   * @param $length
   *   Length of random string to generate.
   *
   * @return
   *   Randomly generated string.
   *
   * @see DrupalWebTestCase::randomString()
   */
  public static function randomName($length = 8) {
    $values = array_merge(range(65, 90), range(97, 122), range(48, 57));
    $max = count($values) - 1;
    $str = chr(mt_rand(97, 122));
    for ($i = 1; $i < $length; $i++) {
      $str .= chr($values[mt_rand(0, $max)]);
    }
    return $str;
  }

  /**
   * Converts a list of possible parameters into a stack of permutations.
   *
   * Takes a list of parameters containing possible values, and converts all of
   * them into a list of items containing every possible permutation.
   *
   * Example:
   * @code
   * $parameters = array(
   *   'one' => array(0, 1),
   *   'two' => array(2, 3),
   * );
   * $permutations = DrupalTestCase::generatePermutations($parameters)
   * // Result:
   * $permutations == array(
   *   array('one' => 0, 'two' => 2),
   *   array('one' => 1, 'two' => 2),
   *   array('one' => 0, 'two' => 3),
   *   array('one' => 1, 'two' => 3),
   * )
   * @endcode
   *
   * @param $parameters
   *   An associative array of parameters, keyed by parameter name, and whose
   *   values are arrays of parameter values.
   *
   * @return
   *   A list of permutations, which is an array of arrays. Each inner array
   *   contains the full list of parameters that have been passed, but with a
   *   single value only.
   */
  public static function generatePermutations($parameters) {
    $all_permutations = array(array());
    foreach ($parameters as $parameter => $values) {
      $new_permutations = array();
      // Iterate over all values of the parameter.
      foreach ($values as $value) {
        // Iterate over all existing permutations.
        foreach ($all_permutations as $permutation) {
          // Add the new parameter value to existing permutations.
          $new_permutations[] = $permutation + array($parameter => $value);
        }
      }
      // Replace the old permutations with the new permutations.
      $all_permutations = $new_permutations;
    }
    return $all_permutations;
  }
}
