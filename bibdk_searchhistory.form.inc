<?php

/**
 * Create the tableselect form with searchresults and possibly saved searches
 *
 * @param type $form
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

  $radios = array(' og ' => t('Combine "And"'), ' eller ' => t('Combine "Or"'));

  $form['bibdk_searchhistory_form']['and_or_radios'] = array(
    '#type' => 'radios',
    '#options' => $radios,
    '#default_value' => ' og ',
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
  if (count($elements) > 0){
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
      if (isset($element))
        BibdkSearchHistory::removeElementFromSession($element);

      // remove from webservice
      if (isset($element['id']))
        BibdkSearchHistory::removeElementFromWebservice($element);
    }
  }
}

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
