<?php

/**
* Implements hook_block_info();
*/
function bibdk_searchhistory_block_info() {
$blocks['bibdk_search_history'] = array(
'info' => t('Bibdk search history'),
'Description' => t('Displaying the current users search history'),
'cache' => DRUPAL_NO_CACHE,
);

return $blocks;
}

/**
 * Implements hook_block_configure();
 */
/*
function bibdk_searchhistory_block_configure($delta) {
  $form = array();
  switch ($delta) {
    case 'bibdk_search_history':
      /* //TODO add pager to table and the ability to change number of results on each page in config
        $form['search_history_count'] = array(
        '#type' => 'textfield',
        '#title' => t('Number of results shown on each page'),
        '#size' => 5,
        '#description' => t('Enter the number of search results shown to the user on each page - 0 = unlimited'),
        '#default_value' => variable_get('search_history_count', 0),
        );
       *
       */
/*
      $form['search_history_save_limit'] = array(
        '#type' => 'textfield',
        '#title' => t('Highest number of saved searches'),
        '#size' => 5,
        '#description' => t('Number of saved searches. When exceeded will the oldest unsaved searches be deleted - 0 = unlimited'),
        '#default_value' => variable_get('search_history_save_limit', 25),
      );
      break;
  }

  return $form;
}
*/


/**
 * Implements hook_block_save().
 */
function bibdk_searchhistory_block_save($delta = '', $edit = array()) {
  switch ($delta) {
    case 'bibdk_search_history':
      #variable_set('search_history_count', (int) abs($edit['search_history_count']));
      variable_set('search_history_save_limit', (int)abs($edit['search_history_save_limit']));
      break;
  }

  return;
}

/**
 * Implements hook_block_view();
 */
function bibdk_searchhistory_block_view($delta = '') {
  switch ($delta) {
    case 'bibdk_search_history':
      $block['subject'] = t('My searches');
      $block['content'] = drupal_get_form('bibdk_searchhistory_form');
      return $block;
      break;
  }
}
