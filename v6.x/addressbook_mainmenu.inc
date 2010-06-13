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
	$_SESSION['fid']="";
	$_SESSION['mid']="";
	
	// Breadcrumb menu
	addressbook_breadcrumb('');

	$page .= '<div class="addressbook">';

	drupal_set_title(t('Addressbook').' '.ADDRESSBOOK_VERSION);

	$page .= '<br/>';
	
	$page .= '<div class="admin-panel">';
	
	$page .= '<H3>';
	$page .= t('Menu');
	$page .= '</H3>';
	$page .= '<br/>';
	
	$page .= '<div class="body">';
	
	$page .= '<dl class="admin-list">';
    
	if (user_access('view addressbook')) {
	
		$page .= '<dt>';
		$page .= l(t('View Family data'),URL_ADDRESSBOOK_FAMILY);
		$page .= '</dt>';
		$page .= '<dd>';
		$page .= t('View family data of addressbook');
		$page .= '</dd>';
  
		$page .= '<dt>';
		$page .= l(t('View Member data'),URL_ADDRESSBOOK_MEMBER);
		$page .= '</dt>';
		$page .= '<dd>';
		$page .= t('View member data of addressbook');
		$page .= '</dd>';
	}
	
	$page .= '<dt>';
	$page .= '&nbsp;';
	$page .= '</dt>';
	
	//only administrators can access these pages.
   if (user_access('administer addressbook')) {
   
		$page .= '<dt>';
		$page .= '<a href="'.url(URL_ADDRESSBOOK_UPLOAD).'">Upload addressbook data</a>';
		$page .= '</dt>';
		$page .= '<dd>';
		$page .= t('Upload addressbook data.');
		$page .= '</dd>';
  
		$page .= '<dt>';
		$page .= '<a href="'.url(URL_ADDRESSBOOK_DOWNLOAD).'">Download addressbook data</a>';
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
	$page .= t('Database information is last updated on').' <b>'.variable_get('addressbook_last_upload_date', '').'</b>';
	$page .= '</div>';
	$page .= '</fieldset>' ;
	
	$page .= '</dl>';
  
	$page .= '</div>';  
	$page .= '</div>';
	$page .= '</div>';

	print theme("page", $page);
}