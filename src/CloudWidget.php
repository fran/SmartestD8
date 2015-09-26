<?php
namespace Drupal\smartest;

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
    $result = $result . ' ' . $this->get_display_term($row1['module'], $this->get_word_index($this->max_size, $row1[$this->size_criteria]),
    $this->get_word_index($this->max_color, $row2[$this->color_criteria]), $description);
  }

    $this->form[$this->title]['#description'] = '<div class="cloud-container"><div class="letter">' . $result . '</div></div>';
    return $this->form;
  }

  function get_display_term($name, $size_index, $color_index, $description) {
    $result = "<span class='tagclouds-term level" . $size_index . " lvlc" . $color_index . "' title='" . $description . "'>" . $name . "</span>\n";
    return $result;
  }

  function get_word_index($range, $value) {
    $result = 1;
    $range_increment = 0.1;

    for ($i = 0; $i < 10; $i++) {
      $starting_range = $i * $range * $range_increment;
      $ending_range = $value < $range * $range_increment * ($i + 1);

      if ($value >= $starting_range && $ending_range) {
        $result = $i + 1;
        break;
      }
    }
    return $result;
  }
}
