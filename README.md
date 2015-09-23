SmarTest
======

SmarTest is a module for improving the testing process in Drupal. It contains new testing techniques to accelerate the detection of faults, providing faster feedback to the testers and reducing debugging efforts.

SmarTest extends the functionality of SimpleTest core module by integrating novel testing techniques and providing the Drupal developer with a dashboard with statistics about the Drupal system in real time. This information allows you to guide the testing in your system through faults propensity data in different parts of your code. Also, SmarTest allows the testers to prioritize (i.e. order) the executions of the Drupal tests modules in order to detect faults as fast as possible basing the prioritization in a research correlation study.

Improvements with respect to SimpleTest
---------------------------------------

-Customizable Dashboard with run-time extracted data to guide the testing
-Different Test prioritizations to detect faults faster
-Automated testing with feedback in real time
-Time-out option to automatically stop the test execution

Libraries
---------
See here which libraries are used within the SmarTest module:
-phploc

External dependencies
---------------------
-SimpleTest
-Charts with Google Charts
-Libraries
-Code coverage (without modifying the Drupal core)

Installation guide
------------------

1- Install SimpleTest module.
2- Download and install Charts module and Google charts (inside of Charts).
3- Download and install Libraries module.
4- Install phploc library:
  - Go to "https://github.com/isa-group/phploc/blob/master/src/Analyser.php".
  - Download the class Analyser.php.
  - Create a folder as:  
      phploc
      |->Analyser.php
  - Locate phploc folder in "Drupal_directory/sites/all/libraries/".
5- Download Code Coverage module. For its installation you'll need:
  5.1 Install XDebug or enable XDebug fastly by uncommenting the following line to [xdebug] -> “zend_extension = ….” in php.ini file of your server. 
    - Restart the server for applying the changes.
  5.2 Code Coverage makes changes in the Drupal Core. If you do not “touch” the Drupal core, do not apply Code Coverage patch.
    - You may apply the patch of SmarTest (smarTest.patch) over Code coverage in order to not touch the Drupal core.
  5.3 Install Code Coverage.
6- Install SmarTest.


Maintainers
-----------

Ana B. Sánchez
Sergio Segura
Antonio Ruiz-Cortés
Gabriel Hidalgo

Supporting organizations: 
-------------------------

Applied Software Engineering (ISA) Funding

Support
-------

For bug reports and feature requests please use the Drupal.org issue tracker:
https://www.drupal.org/project/issues/2503695

