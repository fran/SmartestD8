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
