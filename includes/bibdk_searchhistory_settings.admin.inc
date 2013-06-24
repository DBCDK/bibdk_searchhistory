<?php

/**
 * @file
 * Admin settings for search history.
 */

function bibdk_searchhistory_admin() {
  $form['search_history_save_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of searches saved in session'),
    '#size' => 5,
    '#description' => t('Number of saved searches. When exceeded will the oldest unsaved search be deleted - 0 = unlimited'),
    '#default_value' => variable_get('search_history_save_limit', 25),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('bibdk_searchhistory_admin_submit'),
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for bibdk_searchhistory_admin();
 */
function bibdk_searchhistory_admin_submit($form, &$form_state) {
  $value = $form_state['values']['search_history_save_limit'];
  variable_set('search_history_save_limit', (int) abs($value));
  drupal_set_message(t('Value saved', array(), array('context' => 'bibdk_searchhistory')), 'status');
}
