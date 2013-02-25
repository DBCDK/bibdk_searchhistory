<?php

/**
 * Implements hook_bibdk_mypage_tabs().
 */
function bibdk_searchhistory_bibdk_mypage_tabs($max_rows=3) {
  global $user;

  _bibdk_searchhistory_check_session();

  $logged_in = $user->uid;
  $session_history = array();
  if ($logged_in) {
    $header['memolist'] = t('Memolist');
    $session_history = bibdk_searchhistory_get_saved_searches($logged_in);
  }
  $session_history = bibdk_searchhistory_get_session_searches($logged_in, $session_history);

  $form['bibdk_searchhistory'] = array(
    '#type'         => 'fieldset',
    '#tree'         => TRUE,
    '#theme'        => 'bibdk_mypage-form',
    '#attributes'   => array('class' => array('element-wrapper', 'bibdk-mypage-wrapper')),
  );

  $form['bibdk_searchhistory']['header'] = array(
    '#type'           => 'markup',
    '#markup'         => t('Search history'),
  );

  $form['bibdk_searchhistory']['rows'] = array(
    '#type'           => 'fieldset',
    '#tree'           => TRUE,
    '#empty'          => t('No searches executed yet'),
  );

  if ( sizeof($session_history) > 0 ) {
    $session_history = array_slice($session_history, 0, $max_rows);
    foreach ($session_history as $searchstring => $search) {
      $form['bibdk_searchhistory']['rows'][] = array(
        'item_row'  => array(
          '#type'     => 'markup',
          '#markup'   => $search['search_string'],
         ),
      );
    }
  }
  else {
    $form['bibdk_searchhistory']['rows'][] = array(
      'item_row'  => array(
        '#type'     => 'markup',
        '#markup'   => t('No searches executed yet'),
       ),
    );
  }

  $form['bibdk_searchhistory']['link_profile2_tab'] = array(
    '#type'           => 'link',
    '#title'          => t('Go to search history'),
    '#title_display'  => 'invisible',
    '#href'           => 'user/' . $user->uid . '/edit/bibdk_search_history',
    '#options'        => array('attributes' => array('title' => t('Go to search history'))),
  );

  return $form;
}
