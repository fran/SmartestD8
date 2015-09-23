<?php

/**
 * @file
 * Contains \Drupal\smartest\Form\SmartestDashboardForm.
 */

namespace Drupal\smartest\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class SmartestDashboardForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'smartest_dashboard_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    // @FIXME
// The Assets API has totally changed. CSS, JavaScript, and libraries are now
// attached directly to render arrays using the #attached property.
// 
// 
// @see https://www.drupal.org/node/2169605
// @see https://www.drupal.org/node/2408597
// drupal_add_css(drupal_get_path('module', 'smartest') . '/styles/graph-menu.css');

    $form['#attached']['js'] = [
      drupal_get_path('module', 'smartest') . '/smartest.js'
      ];

    $library = libraries_load('gridster');

    $query_page = db_select('smartest_cache')
      ->fields('smartest_cache', ['type'])
      ->condition('cookie', 'dashboard_page', '=')
      ->execute()
      ->fetchAssoc();

    $query_all_pages = db_select('smartest_widget')
      ->fields('smartest_widget', ['dashboard_name'])
      ->execute();
    $pages = [];

    while ($row = $query_all_pages->fetchAssoc()) {
      if (!in_array($row['dashboard_name'], $pages)) {
        $pages[$row['dashboard_name']] = $row['dashboard_name'];
      }
    }

    $form['dashboard'] = [
      '#type' => 'fieldset',
      '#title' => t('Dashboard'),
    ];
    $form['dashboard']['update'] = [
      '#type' => 'submit',
      '#value' => t('Update statistics'),
      '#attributes' => [
        'class' => [
          'update'
          ]
        ],
      '#submit' => ['update_statistics'],
    ];
    $form['dashboard']['profile'] = [
      '#type' => 'fieldset',
      '#title' => t('Profile'),
    ];
    $form['dashboard']['profile']['select'] = [
      '#type' => 'select',
      '#options' => $pages,
      '#default_value' => $query_page['type'],
    ];
    $form['dashboard']['profile']['load'] = [
      '#type' => 'submit',
      '#value' => t('Load'),
      '#submit' => [
        'smartest_load_submit'
        ],
    ];
    $form['dashboard']['profile']['new_dashboard'] = [
      '#type' => 'button',
      '#value' => t('New dashboard'),
      '#ajax' => [
        'callback' => 'smartest_add_page_callback',
        'wrapper' => 'add-page',
        'effect' => 'fade',
      ],
    ];
    $form['dashboard']['profile']['addWidget'] = [
      '#type' => 'button',
      '#value' => t('Add widget'),
      '#ajax' => [
        'callback' => 'smartest_add_widget_callback',
        'wrapper' => 'add-widget',
        'effect' => 'fade',
      ],
    ];
    // Add a new widget form
    $form['dashboard']['add-widget'] = [
      '#prefix' => '<div id="add-widget">',
      '#suffix' => '</div>',
    ];
    if ($form_state->getValue(['addWidget'])) {

      $form['add-widget'] = [
        '#type' => 'fieldset',
        '#title' => t('Add Widget'),
      ];
      $form['add-widget']['title'] = [
        '#type' => 'textfield',
        '#title' => t('Widget title') . ' <fail>*</fail>',
      ];
      $form['add-widget']['widget-type'] = [
        '#type' => 'select',
        '#title' => t('Select a widget type'),
        '#options' => [
          'chart' => t('Graphic'),
          'cloud' => t('Cloud'),
        ],
        '#default_value' => 'chart',
        '#ajax' => [
          'callback' => 'smartest_active_div_callback',
          'wrapper' => 'wrapper-act',
        ],
      ];
      $form['add-widget']['wrapper'] = [
        '#prefix' => '<div id="wrapper-act">',
        '#suffix' => '</div>',
      ];
      $form['add-widget']['wrapper']['clouds'] = [
        '#type' => 'fieldset',
        '#title' => 'Clouds configurations',
        '#access' => $form_state->getValue(['widget-type']) == 'cloud',
      ];
      $form['add-widget']['wrapper']['clouds']['selectcriteria1'] = [
        '#type' => 'select',
        '#title' => t('Select size criteria'),
        '#options' => [
          'cc' => t('Ciclomatic complexity'),
          'loc' => t('Lines of code'),
          'fails' => t('Failed tests'),
          'exceptions' => t('Tests with exceptions'),
          'test_execution_time' => t('Execution time'),
          'passes' => t('Passed tests'),
          'coverage' => t('Coverage'),
          'git_changes' => t('Git changes'),
        ],
        '#default_value' => 'loc',
      ];
      $form['add-widget']['wrapper']['clouds']['selectcriteria2'] = [
        '#type' => 'select',
        '#title' => t('Select color criteria'),
        '#options' => [
          'cc' => t('Ciclomatic complexity'),
          'loc' => t('Lines of code'),
          'fails' => t('Failed tests'),
          'exceptions' => t('Tests with exceptions'),
          'test_execution_time' => t('Execution time'),
          'passes' => t('Passed tests'),
          'coverage' => t('Coverage'),
          'git_changes' => t('Git changes'),
        ],
        '#default_value' => 'cc',
      ];
      $form['add-widget']['wrapper']['graphics'] = [
        '#type' => 'fieldset',
        '#title' => 'Graphics configurations',
      ];
      $form['add-widget']['wrapper']['graphics']['type'] = [
        '#type' => 'select',
        '#title' => t('Select graphics types'),
        '#options' => [
          'column' => t('Column'),
          'line' => t('Line'),
        ],
      ];
      $form['add-widget']['wrapper']['graphics']['selectcriteria'] = [
        '#type' => 'select',
        '#title' => t('Select criteria'),
        '#options' => [
          'cc' => t('Ciclomatic complexity'),
          'loc' => t('Lines of code'),
          'test' => t('Test Results'),
          'test_execution_time' => t('Execution time (seconds)'),
          'passes' => t('Passed tests'),
          'coverage' => t('Coverage'),
          'git_changes' => t('Git changes'),
        ],
      ];
      $form['add-widget']['selectmodules'] = [
        '#type' => 'checkboxes',
        '#title' => t('Select modules'),
        '#options' => [
          'ALL' => t('All'),
          'top5' => 'Top 5',
          'top10' => 'Top 10',
        ],
        '#default_value' => ['top5'],
      ];
      $emods = module_list();
      $count = 1;
      foreach ($emods as $mod => $value) {
        $form['add-widget']['selectmodules']['#options'][$value] = t($mod);
        $count++;
      }
      $form['add-widget']['submit-form'] = [
        '#type' => 'submit',
        '#value' => t('Generate'),
        '#submit' => [
          'smartest_add_widget_form_submit'
          ],
      ];
      $form['add-widget']['cancel-submit-form'] = [
        '#type' => 'submit',
        '#value' => t('Cancel'),
      ];

    }

    $form['dashboard']['add-page'] = [
      '#prefix' => '<div id="add-page">',
      '#suffix' => '</div>',
    ];
    if ($form_state->getValue(['new_dashboard'])) {
      $form['add-page'] = [
        '#type' => 'fieldset',
        '#title' => 'Add new dashboard',
      ];
      $form['add-page']['title-page'] = [
        '#type' => 'textfield',
        '#title' => t('Dashboard title'),
      ];
      $form['add-page']['submit-form'] = [
        '#type' => 'submit',
        '#value' => t('+ Add'),
        '#submit' => [
          'smartest_add_page_form_submit'
          ],
      ];
      $form['add-page']['cancel-submit-form'] = [
        '#type' => 'submit',
        '#value' => t('Cancel'),
      ];
    }

    $form['dashboard']['wg'] = [
      '#prefix' => '<div class="gridster">',
      '#suffix' => '</div>',
    ];

    $actual_value = $form['dashboard']['profile']['select']['#default_value'];
    $wgt_query = db_select('smartest_widget')
      ->fields('smartest_widget', [
      'title',
      'widget_type',
      'criteria1',
      'criteria2',
      'modules',
      'n_element',
      'type',
    ])
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

}
