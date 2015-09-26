<?php

/**
 * Global variable that holds information about the tests being run.
 *
 * An array, with the following keys:
 *  - 'test_run_id': the ID of the test being run, in the form 'simpletest_%"
 *  - 'in_child_site': TRUE if the current request is a cURL request from
 *     the parent site.
 *
 * @var array
 */
global $drupal_test_info;

/**
 * Logs verbose message in a text file.
 *
 * If verbose mode is enabled then page requests will be dumped to a file and
 * presented on the test result screen. The messages will be placed in a file
 * located in the simpletest directory in the original file system.
 *
 * @param $message
 *   The verbose message to be stored.
 * @param $original_file_directory
 *   The original file directory, before it was changed for testing purposes.
 * @param $test_class
 *   The active test case class.
 *
 * @return
 *   The ID of the message to be placed in related assertion messages.
 *
 * @see DrupalTestCase->originalFileDirectory
 * @see DrupalWebTestCase->verbose()
 */
function simpletest_verbose($message, $original_file_directory = NULL, $test_class = NULL) {
  static $file_directory = NULL, $class = NULL, $id = 1, $verbose = NULL;

  // Will pass first time during setup phase, and when verbose is TRUE.
  if (!isset($original_file_directory) && !$verbose) {
    return FALSE;
  }

  if ($message && $file_directory) {
    $message = '<hr />ID #' . $id . ' (<a href="' . $class . '-' . ($id - 1) . '.html">Previous</a> | <a href="' . $class . '-' . ($id + 1) . '.html">Next</a>)<hr />' . $message;
    file_put_contents($file_directory . "/simpletest/verbose/$class-$id.html", $message, FILE_APPEND);
    return $id++;
  }

  if ($original_file_directory) {
    $file_directory = $original_file_directory;
    $class = $test_class;
    // @FIXME
    // This looks like another module's variable. You'll need to rewrite this call
    // to ensure that it uses the correct configuration object.
    // $verbose = variable_get('simpletest_verbose', TRUE);

    $directory = $file_directory . '/simpletest/verbose';
    $writable = file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
    if ($writable && !file_exists($directory . '/.htaccess')) {
      file_put_contents($directory . '/.htaccess', "<IfModule mod_expires.c>\nExpiresActive Off\n</IfModule>\n");
    }
    return $writable;
  }
  return FALSE;
}
