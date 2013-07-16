<?php

/**
 * @file
 * Create the tableselect form with searchresults and possibly saved searches
 *
 * @param array $form
 * @param $form_state
 * @return array $form
 */
function bibdk_searchhistory_form($form, $form_state) {

  drupal_set_title(t('Searchhistory'));
  
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

  $form['actions'] = bibdk_searchhistory_get_primary_actions(array());

  $form['bibdk_searchhistory_form']['bibdk_search_history_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $search_views,
    '#empty' => array('#markup' => '' . t('No searches executed yet')),
    '#js_select' => FALSE,
    '#sticky' => FALSE,
    '#multiple' => TRUE,
    '#attributes' => array(
      'class' => array(
        'table',
        'combine-select',
        'element-wrapper',
        'search-history-table',
      ),
    ),
  );
  $op_and = _bibdk_searchhistory_get_cql('AND');
  $op_or = _bibdk_searchhistory_get_cql('OR');;

  $radios = array($op_and => t('Combine "And"'), $op_or => t('Combine "Or"'));

  $form['bibdk_searchhistory_form']['and_or_radios'] = array(
    '#type' => 'radios',
    '#options' => $radios,
    '#default_value' => $op_and,
  );

  $form['bibdk_searchhistory_form']['combine'] = array(
    '#type' => 'submit',
    '#value' => t('Combiner'),
    '#submit' => array('bibdk_searchhistory_form_combine'),
    '#attributes' => array(
      'class' => array(
        'search-action-list'
      ),
      'disabled' => TRUE
    ),
  );

  //Adding JavaScript
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'bibdk_searchhistory') . '/js/bibdk_searchhistory.js',
  );

  return $form;
}

/** Wrapper for ting_openformat_get_cql_commando
 * see ting_openformat.module
 * @param $cql
 * @return string
 */
function _bibdk_searchhistory_get_cql($cql) {
  return function_exists('ting_openformat_get_cql_commando') ? ting_openformat_get_cql_commando($cql) : ' ' . $cql . ' ';
}

function bibdk_searchhistory_get_primary_actions() {

  $elements = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'search-action-list',
      ),
    ),
  );

  $elements['select_all'] = array(
    '#type' => 'link',
    '#title' => t('mark_all_none', array(), array('context' => 'bibdk_cart')),
    '#title_display' => 'invisible',
    '#href' => '',
    '#options' => array('attributes' => array(
      'class' => array(
        'select-all'
      ),
      'title' => t('Go to cart')),
    ),

  );

  $elements['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete selected'),
    '#submit' => array('bibdk_searchhistory_form_delete'),
    '#attributes' => array(
      'disabled' => TRUE,
      'class' => array(
        'button-as-link',
      ),
    ),
    '#ajax' => array(
      'callback' => 'bibdk_searchhistory_form_delete',
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'bibdk_searchhistory') . '/js/bibdk_searchhistory.ajax.js'),
    ),
  );

  return $elements;
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
  $commands = array();
  foreach ($searches as $search => $string) {
    if ($search) {
      $element = BibdkSearchHistory::getElement($string);
      $commands[] = bibdk_searchhistory_ajax_command('input[value=' . $string . ']');
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
  return array('#type' => 'ajax', '#commands' => $commands);
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
      $combined[] = _bibdk_searchhistory_format_combine_string($string);
    }
  }

  $new_string = implode($combinator, $combined);

  drupal_goto('search/work/' . $new_string);
}

/**
 * Helper function to add enclosing parentheses to strings with spaces and/or parentheses
 *
 * Parentheses in input string are already balanced.
 *
 * @param $string
 * @return string
 */
function _bibdk_searchhistory_format_combine_string($string) {
  $stack = array();
  $tokens = preg_split('/([() ])/', trim($string), 0, PREG_SPLIT_DELIM_CAPTURE);

  foreach ($tokens as $token) {
    if ($token == ')') {
      while (array_pop($stack) != '(') {}
    }
    else {
      array_push($stack, $token);
    }
  }

  if (count($stack) == 1 || count($stack) == 2 && $stack[0] == '' && $stack[1] == '') {
    return $string;
  }
  else {
    return '(' . $string . ')';
  }
}

/** Generates a view for each element in the search history
 *
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
 *
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
  $saved = isset($element['id']) ? TRUE : FALSE;
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
 *
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
 *
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
