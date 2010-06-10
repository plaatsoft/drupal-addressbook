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
 
// *********************************************************************************
// StateMachine
// *********************************************************************************
 
function addressbook_member($fid=0) {

	//echo var_dump($_POST);
	
	// *****************************
	// Process POST an SESSION variables
	// *****************************
	
	$Action=htmlentities($_POST['Action']);
	addressbook_debug_param("Action", $Action);
		
	// *****************************
	// Process statemachine
	// *****************************
	
	switch ($Action) {
				
		default: 
			$page = addressbook_member_list();
			break;
	}
	
	if ($page!="") {
		print theme("page", $page);
	}
}
			
			
// *********************************************************************************
// Views
// *********************************************************************************

/**
 * Render a page showing the selected address for editing
 * @return HTML
 */
function addressbook_member_list( $sort='' )  {

	// *****************************
	// Get posted variables
	// *****************************
	
	$Sort = $_POST["Sort"];
	if ($Sort=="") {
		$Sort=$_SESSION["Sort"];
		if ($Sort=="") {
			$Sort="A";
		}
	}
	$_SESSION["Sort"]=$Sort;
	addressbook_debug_param("Sort", $Sort);

	$Search=$_POST["Search"];
	addressbook_debug_param("Search", $Search);
  
	// *****************************
	// Query Database
	// *****************************
	
	if ($Search!="") {
		$parameter=$Search;
	} else {
	   $parameter=$Sort;
	}
	
	// Fetch member from database
  $query  = '
		SELECT 
			a.mid,
			a.first_name, 
			a.middle_name, 
			a.last_name, 
			a.mobile, 
			a.email, 
			a.mid, 
			b.fid, 
			a.uid, 
			b.telephone,
			c.picture as picture
		FROM 
			addressbook_member a, addressbook_family b LEFT JOIN addressbook_picture c ON c.mid=a.mid 
		WHERE 
			a.fid=b.fid and
			a.last_name LIKE "'.$parameter.'%" ';
	$query .= 'ORDER BY a.last_name, a.middle_name ';
	
	$queryResult = db_query($query);

	// *****************************
	// View
	// *****************************

	drupal_set_title(t('Addressbook member list'));
  
	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';

	// Breadcrumb menu
	$node["child1"] = t("Member list");
	addressbook_breadcrumb($node);
  
  	$page .= '<fieldset>' ;
	$page .= '<legend>'.t('Filteren').'</legend>';
	
	$page .= addressbook_form_item(
					t('Achternaam:'),
					addressbook_view_text("Search", 40, 40, $search, false).
					addressbook_hiddenlink('AddressbookForm','','','Search',t('Search')),
					t("Enter lastname and press enter to find the member."), 
					$readonly );
					
	$page .= addressbook_sortbar($Sort,"List");
	
	$page .= '</fieldset>';
 
	$page .= '<table>';
	$page .= '<thead>';

	$page .= '<tr>';
	if (user_access('access administration pages')) {
		$page .= '<th>';
		$page .= t('Id');
		$page .= '</th>';
	}
		
	if ( variable_get('addressbook_show_thumbnails',0)==1 ) {
		$page .= '<th>';
		$page .= '<b>'.t('Picture').'</b>';
		$page .= '</th>';
	}
	$page .= '<th>';
	$page .= '<b>'.t('Name').'</b>';
	$page .= '</th>';

	$page .= '<th>';
	$page .= '<b>'.t('Telephone').'</b>';
	$page .= '</th>';

	$page .= '<th>';
	$page .= '<b>'.t('Mobile').'</b>';
	$page .= '</th>';

	$page .= '<th>';
	$page .= '<b>'.t('Email').'</b>';
	$page .= '</th>';

	$page .= '</tr>';	
	$page .= '</thead>';
	$page .= '<tbody>';
	
	// Show all found members
	$page_tmp='';
	while ($data = db_fetch_object($queryResult)) {
	
		if ((++$i%2)==0) {
			$page_tmp .= '<tr class="even">';
		} else {
			$page_tmp .= '<tr class="odd">';
		}
		
		if (user_access('access administration pages')) {
			$page_tmp .= '<td>';
			$page_tmp .= $data->mid;
			$page_tmp .= '</td>';
		}

		if ( variable_get('addressbook_show_thumbnails',0)==1 ) {
			$page .= '<td>';
			$page .= addressbook_load_picture($data->picture, 30, 25, "left", false);
			$page .= '</td>';
		}
		     
		// Format Name
		$page_tmp .= '<td>';
		$name=addressbook_view_name($data->first_name,$data->middle_name,$data->last_name,true);
		$page_tmp .= addressbook_hiddenlink('AddressbookForm', 'mid', $data->mid, 'MemberView', $name);
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->telephone;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->mobile;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= l($data->email,'mailto:'.$data->email);
		$page_tmp .= '</td>';
		
		$page_tmp .= '</tr>';
	}

	if ( $page_tmp!='') {
     $page .= $page_tmp;
	} else {
		// No content found
		$page .= '<tr>'; 
		$page .= t('No records found');
		$page .= '</tr>';
  }
  $page .= '</table>';
  
  $page .= '</div>';
  $page .= '</form>'; 
  
  return $page;
}

// *********************************************************************************
// The end
// *********************************************************************************
