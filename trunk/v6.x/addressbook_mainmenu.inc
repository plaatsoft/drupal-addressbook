<?php

/** 
 *  Addressbook module for drupal 
 *
 *  Copyright (C) 2006-2010
 *  =======================
 *
 *  Created by wplaat
 *
 *  For more information visit the following website.
 *  Website : http://www.plaatsoft.nl 
 *
 *  Or send an email to the following address.
 *  Email   : info@plaatsoft.nl
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

function addressbook_mainmenu() {

   //global user;
	
	// Clear session scope
	$_SESSION['Action']="";
	$_SESSION['Sort']="";
	$_SESSION['Search']="";
	
	// Breadcrumb menu
	oostpoort_breadcrumb('');

	$page .= '<div class="oostpoort">';

	drupal_set_title(t('Addressbook Menu').' '.ADDRESSBOOK_VERSION);

	$page .= '<br/>';
	
	$page .= '<div class="admin-panel">';
	$page .= '<H3>';
	$page .= t('Addressbook menu');
	$page .= '</H3>';
	$page .= '<br/>';
	
	$page .= '<div class="body">';
	
	$page .= '<dl class="admin-list">';
    
	$page .= '<dt>';
	$page .= '<a href="'.url(URL_FAMILY_LIST).'">Find family</a>';
	$page .= '</dt>';
	$page .= '<dd>';
	$page .= t('Find family in addressbook');
	$page .= '</dd>';
  
	$page .= '<dt>';
	$page .= '<a href="'.url(URL_MEMBER_LIST).'">Find member</a>';
	$page .= '</dt>';
	$page .= '<dd>';
	$page .= t('Find member in addressbook.');
	$page .= '</dd>';
  
	$page .= '<dt>';
	$page .= '&nbsp;';
	$page .= '</dt>';
	
	//only administrators can access these pages.
   if (user_access('access administration pages')) {
   
		$page .= '<dt>';
		$page .= '<a href="'.url(URL_UPLOAD_CSV).'">Upload addressbook data</a>';
		$page .= '</dt>';
		$page .= '<dd>';
		$page .= t('Upload addressbook data.');
		$page .= '</dd>';
  
		$page .= '<dt>';
		$page .= '<a href="'.url(URL_DOWNLOAD_CSV).'">Download addressbook data</a>';
		$page .= '</dt>';
		$page .= '<dd>';
		$page .= t('Download addressbook data.');
		$page .= '</dd>';
	}

	$page .= '<dt>';
	$page .= '&nbsp;';
	$page .= '</dt>';
		
	$page .= '<fieldset>' ;	
	$page .= '<legend>'.t('Note').'</legend>';
	$page .= '<div class="description">';
	$page .= t('Database information is last update on <b>'.variable_get('addressbook_last_upload_date', '').'</b>');
	$page .= '</div>';
	$page .= '</fieldset>' ;
	
	$page .= '</dl>';
  
	$page .= '</div>';  
	$page .= '</div>';
	$page .= '</div>';

	print theme("page", $page);
}