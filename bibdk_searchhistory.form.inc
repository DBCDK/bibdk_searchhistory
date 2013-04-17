<?php

/**
 * Create the tableselect form with searchresults and possibly saved searches
 *
 * @param array $form
 * @param $form_state
 * @return array $form
 */
function bibdk_searchhistory_form($form, $form_state) {

  $elements = BibdkSearchHistory::getAllElements();
  $search_views = bibdk_searchhistory_get_views($elements);
  $form['#attributes']['class'] = array(
    'searchhistory',
  );

  $header = array(
    'search_string' => t('Search string'),
    'result_count' => t('Result count'),
    'memolist' => t('Save'),
  );

  $form['bibdk_searchhistory_form']['bibdk_search_history_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $search_views,
    '#empty' => t('No searches executed yet'),
    '#js_select' => TRUE,
    '#tree' => TRUE,
    '#multiple' => TRUE,
    '#default_value' => array(),
    '#attributes' => array(
      'class' => array(
        'combine-select'
      ),
    ),
  );
  $op_and = (variable_get('cglLang', 'cqleng') == 'cqleng') ? ' AND ' : ' OG ';
  $op_or = (variable_get('cglLang', 'cqleng') == 'cqleng') ? ' OR ' : ' ELLER ';

  $radios = array($op_and => t('Combine "And"'), $op_or => t('Combine "Or"'));

  $form['bibdk_searchhistory_form']['and_or_radios'] = array(
    '#type' => 'radios',
    '#options' => $radios,
    '#default_value' => $op_and,
  );

  $form['bibdk_searchhistory_form']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete selected'),
    '#submit' => array('bibdk_searchhistory_form_delete'),
    '#attributes' => array(
      'disabled' => TRUE
    ),
  );
  $form['bibdk_searchhistory_form']['combine'] = array(
    '#type' => 'submit',
    '#value' => t('Combiner'),
    '#submit' => array('bibdk_searchhistory_form_combine'),

    '#attributes' => array(
      'disabled' => TRUE
    ),
  );

  //Adding JavaScript
  if (count($elements) > 0) {
    $form['#attached']['js'] = array(
      drupal_get_path('module', 'bibdk_searchhistory') . '/js/bibdk_searchhistory.js',
    );
  }

  return $form;
}

/**
 * form callback function. Removes selected entries from $_SESSION['searches] array and deletes posts in db if necessary
 *
 * @global Array $user
 * @param array $form
 * @param array $form_state
 */
function bibdk_searchhistory_form_delete($form, &$form_state) {
  $searches = $form_state['values']['bibdk_search_history_table'];
  foreach ($searches as $search => $string) {
    if ($search) {
      $element = BibdkSearchHistory::getElement($string);
      // Remove from session
      if (isset($element)) {
        BibdkSearchHistory::removeElementFromSession($element);
      }

      // remove from webservice
      if (isset($element['id'])) {
        BibdkSearchHistory::removeElementFromWebservice($element);
      }
    }
  }
}

/**
 * @param array $form
 * @param array $form_state
 */
function bibdk_searchhistory_form_combine($form, $form_state) {
  $searches = $form_state['values']['bibdk_search_history_table'];
  $combined = array();
  $combinator = $form_state['values']['and_or_radios'];
  foreach ($searches as $search => $string) {
    if ($string) {
      $combined[] = $string;
    }
  }
  $new_string = $combined[0];
  for ($i = 1; $i < count($combined); $i++) {
    $new_string .= $combinator;
    $new_string .= $combined[$i];
  }
  drupal_goto('search/work/' . $new_string);
}


/** Generates a view for each element in the search history
 * @param $element
 * @return array
 */
function _bibdk_searchhistory_element_view($element) {
  $string = $element['string'];
  $count = $element['count'];
  $memolist = bibdk_searchhistory_action_link($element);
  return array(
    'search_string' => l($string, 'search/work/' . $string),
    'result_count' => $count,
    'memolist' => drupal_render($memolist),
  );
}

/** Create an add remove button for logged in users
 * @param $element
 * @return array
 */
function bibdk_searchhistory_action_link($element) {
  global $user;
  if (!ding_user_is_provider_user($user)) {
    return array(
      '#type' => 'markup',
      '#markup' => t('login to save'),
    );
  }
  static $c;
  $c++;
  $saved = isset($element['id']) ? true : false;
  $serial_element = urlencode(serialize($element));
  $render = array(
    '#type' => 'link',
    '#href' => '',
    '#title' => $saved ? t('Remove') : t('Add'),
    '#options' => array(
      'attributes' => array(
        'element-id' => $c,
      ),
    ),
    '#ajax' => array(
      'path' => 'bibdk_searchhistory/ajax/' . $c . '/' . $serial_element,
      'callback' => 'bibdk_searchhistory_action_link_callback',
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'bibdk_searchhistory') . '/js/bibdk_searchhistory.ajax.js'),
    ),
  );
  return $render;
}


/** Ajax callback : add/remove search history element from webservice
 * @param $count
 * @param $serial_element
 */
function bibdk_searchhistory_action_link_callback($count, $serial_element) {
  $element = unserialize(urldecode($serial_element));
  if (!isset($element['id'])) {
    $element = BibdkSearchHistory::addElementToWebservice($element);
    $link = bibdk_searchhistory_action_link($element);
    $commands[] = ajax_command_replace('[element-id=' . $count . ']', drupal_render($link));
  }
  else {
    $result = BibdkSearchHistory::removeElementFromWebservice($element); //xxx
    $commands[] = bibdk_searchhistory_ajax_command('[element-id=' . $count . ']');
  }

  $callback = array('#type' => 'ajax', '#commands' => $commands);
  ajax_deliver($callback);
}

/** Ajax command to remove element from dom
 * @param $selector
 * @return array
 */
function bibdk_searchhistory_ajax_command($selector) {
  return array(
    'command' => 'searchhistory_remove',
    'selector' => $selector,
  );
}

/** Generate views from searchhistory elements
 *
 * @param $elements
 * @return array
 */
function bibdk_searchhistory_get_views($elements) {
  $views = array();
  foreach ($elements as $element) {
    $views[$element['string']] = _bibdk_searchhistory_element_view($element);
  }
  return $views;
}
