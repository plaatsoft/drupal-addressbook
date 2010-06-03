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
 
function addressbook_family($fid=0) {

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
   
		case 'FamilyList':
			$page = addressbook_family_list();
			break;
			
		case 'FamilyView':
			$page = addressbook_family_view();
			break;
			
		case 'FamilyEdit':
			$page = addressbook_family_edit();
			break;
			
		case 'FamilyInsert':
			addressbook_save_picture();
			addressbook_family_insert();
			$page = addressbook_family_view();
			break;
			
		case 'FamilyUpdate':
			addressbook_save_picture();
			addressbook_family_update();
			$page = addressbook_family_view();
			break;
			
		case 'FamilyDelete':
			addressbook_family_delete();
			$page = addressbook_family_list();
			break;
			
		case 'MemberAdd':
			$page = addressbook_member_view(false);
			break;
				
		case 'MemberEdit': 
			$page = addressbook_member_view(false);
			break;
			
		case 'MemberView': 
			$page = addressbook_member_view(true);
			break;
			
		case 'MemberList': 
			$page = addressbook_member_list();
			break;
			
		case 'MemberInsert': 
			addressbook_insert_member();
			$page = addressbook_family_view();
			break;
			
		case 'MemberUpdate': 
			addressbook_update_member();
			$page = addressbook_family_view();
			break;
		
		case 'MemberDelete': 
			addressbook_delete_member();
			$page = addressbook_family_view();
			break;
			
		case 'PictureDelete': 
			addressbook_picture_delete();
			$page = addressbook_family_edit();
			break;
			
		default: 
			$page = addressbook_family_list();
			break;
	}
	
	if ($page!="") {
		print theme("page", $page);
	}
}
			
			
// *********************************************************************************
// Views
// *********************************************************************************

/*
 * Function shows all family in a list form
 * @return HTML
 */
function addressbook_family_list() {

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
	addressbook_debug_param("Sort", $Sort);
	$_SESSION["Sort"]=$Sort;

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
	
	$query  = '
		SELECT 
			fid, 
			middle_name, 
			last_name, 
			street, 
			zipcode, 
			city, 
			uid
		FROM 
			{addressbook_family} 
		WHERE 
			LAST_NAME LIKE "'.$parameter.'%" 
		ORDER BY LAST_NAME, MIDDLE_NAME';
  
	$queryResult = db_query($query);

	// *****************************
	// Create page
	// *****************************
	
	drupal_set_title(t('Addressbook family list'));
  
	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
  
  	$page .= '<fieldset>' ;
	$page .= '<legend>'.t('Filteren').'</legend>';
	
	$page .= '<div class="form-item">';
	$page .= '<label>'.t('Achternaam').' :</label>';
	$page .= '<input id="text" name="Search" size="40" maxlength="40" type="text" value="'.$Search.'"> ';
	$page .= addressbook_hiddenlink('AddressbookForm','','','Search',t('Search'));
	$page .= '<div class="description">';
	$page .= t('Enter lastname and press enter to find a family.');
	$page .= '</div>';
	$page .= '</div>';
	
	$page .= addressbook_sortbar($Sort,"List");
	  
	$page .= '</fieldset>';

	$page .= '<table>';
	$page .= '<thead>';
  
	$page .= '<tr>';
  
	if ( variable_get('addressbook_show_thumbnails',0)==1) {
		$page .= '<th>';
      $page .= '<b>'.t('Picture').'<b>';
      $page .= '</th>';
	}
	$page .= '<th>';
	$page .= '<b>'.t('Name').'</b>';
	$page .= '</th>';
	
	$page .= '<th>';
	$page .= '<b>'.t('Street').'</b>';
	$page .= '</th>';
	
	$page .= '<th>';
	$page .= '<b>'.t('Zipcode').'</b>';
	$page .= '</th>';
	
	$page .= '<th>';
	$page .= '<b>'.t('City').'</b>';
	$page .= '</th>';
	
	$page .= '</tr>';
	
	$page .= '</thead>';
	$page .= '<tbody>';

	// Show all found families
	$page_tmp='';
	while ($data = db_fetch_object($queryResult)) {

		if ((++$i%2)==0) {
			$page_tmp .= '<tr class="even">';
		} else {
			$page_tmp .= '<tr class="odd">';
		}
	
		if ( variable_get('addressbook_show_thumbnails',0)==1) {

			$page_tmp .= '<td>';

			// Query sort found pictures on birtday
			$query2 = 'SELECT picture FROM {addressbook_picture} WHERE fid="'.$data->fid.'" AND mid="0"';
			$queryResult2 = db_query($query2);
			if ($tmp = db_fetch_object($queryResult2)) {
				// Show first picture with is found
				$page_tmp .= '<a href="'.url(URL_FAMILY_VIEW.'/'.$data->fid).'">';
				$filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
				$filename=str_replace(array("?q="), "", $filename);
				$page_tmp .='<img src="'.$filename.'" width=60 height=50/></a>';
			}	
			
			$page_tmp .= '</td>';
		}
	
		// Format Name
		$page_tmp .= '<td>';
		if ( variable_get('addressbook_name_format',1)=='1') {
			$name = $data->middle_name.' '.$data->last_name;
		} else {
			$name = $data->last_name.', '.$data->middle_name;
		}
		$page_tmp .= oostpoort_hiddenlink('AddressbookForm', 'fid', $data->fid, 'FamilyView', $name);
		$page_tmp .= '</td>';	
	
		$page_tmp .= '<td>';
		$page_tmp .= $data->street;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->zipcode;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->city;
		$page_tmp .= '</td>';
		
		$page_tmp .= '</tr>';
	}

	if ( $page_tmp!='') {

		// Show Content
		$page .= $page_tmp;
		
	} else {
	
		// No content found
		$page .= '<tr>';
		$page .= '<td>';
		$page .= t('No records found');
		$page .= '</td>';
		$page .= '</tr>';
	}
	$page .= '</tbody>';
	$page .= '</table>';
 
	$page .= addressbook_hiddenlink('AddressbookForm','fid',0,'FamilyEdit',t('New family'));
	$page .= ' | '.l(t("Back"), URL_ADDRESSBOOK);
	
	$page .= '</div>';
	$page .= '</form>';
	
	return $page;
}
  
/**
 * Render a page showing the selected family in detail
 * @return HTML
 */
function addressbook_family_view( $fid = -1)  {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	if ($fid==-1) {
		$fid=$_POST["fid"];
		if ($fid=="") {
			$fid=$_SESSION["fid"];
		}
	}
	addressbook_debug_param("fid", $fid);
	
	$_SESSION['fid']=$fid;
		
	// *****************************
	// Query Database
	// *****************************

	$query  = '
		SELECT 
			fid, 
			middle_name, 
			last_name, 
			street, 
			zipcode, 
			city, 
			country, 
			telephone, 
			uid 
		FROM 
			{addressbook_family} 
		WHERE 
			FID='.$fid;
	
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook family view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
	
	//addressbook_save_picture("fid",$fid);
						
	$page .= '<fieldset>' ;
	$page .= '<legend></legend>';

	//$page .= addressbook_picture("Adr",$fid, 0, 0,"right", true);
	
	$page .= '<h4>'.$data->middle_name.' '.$data->last_name.'</h4>';
	$page .= '<h4>'.$data->street.'</h4>';	
	$page .= '<h4>'.$data->zipcode.'&nbsp;&nbsp;'.$data->city.'</h4>';	
	$page .= '<h4>'.$data->country.'</h4>';	
	$page .= '<h4>'.$data->telephone.'</h4>';	
	
	// Query Family Members
	$query  = '
		SELECT 	
			mid, 
			first_name, 
			middle_name, 
			last_name, 
			birth_day, 
			mobile, 
			email, 
			uid 
		FROM 
			{addressbook_member} 
		WHERE 
			fid='.$fid.' 
		ORDER BY birth_day';
		
	$queryResult = db_query($query);
		
	// Show Banner
	$page .= '<table>';
	$page .= '<thead>';
	$page .= '<tr>';
	$page .= '<th><b>'.t('Phote').'</b></th>';
	$page .= '<th><b>'.t('Name').'</b></th>';	
	$page .= '<th class="active"><b>'.t('Birthday').'</b></th>';
	$page .= '<th><b>'.t('Mobiel').'</b></th>';
	$page .= '<th><b>'.t('Email').'</b></th>';	
	$page .= '</tr>';
	$page .= '</thead>';
	
	// Show all found members
	$page .= '<tbody>';
	$page_tmp='';
	while ($data = db_fetch_object($queryResult)) {

		if ((++$i%2)==0) {
			$page .= '<tr class="even">';
		} else {
			$page .= '<tr class="odd">';
		}

		if ( variable_get('addressbook_show_thumbnails',0)==1 ) {
			$query2  = 'SELECT pid, picture FROM {addressbook_picture} WHERE fid="'.$fid.'" AND mid="'.$data->mid.'"';
			$queryResult2 = db_query($query2);
			$tmp = db_fetch_object($queryResult2);

			$page .= '<td>';
			
			if ($tmp!=null) {	
				// Show each picture with is found
				$filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
				$filename=str_replace(array("?q="), "", $filename);
				$page .= '<img src="'.$filename.'" width=25 height=30/>';
			} 
			$page .= '</td>';
      }
	
		$page .= '<td>';
		$name=addressbook_view_name($data->first_name,$data->middle_name,$data->last_name,true);
		$page .= addressbook_hiddenlink('AddressbookForm','mid',$data->mid,'MemberView',$name);
		$page .= '</td>';
		
		$page .= '<td>';
		list($year, $month, $day) = split('[/.-]', $data->birth_day);
		$page .= $day.'-'.$month.'-'.$year;
		$page .= '</td>';

		$page .= '<td>';
		$page .= $data->mobile;
		$page .= '</td>';
		
		$page .= '<td>';
		$page .= l($data->email,'mailto:'.$data->email);
		$page .= '</td>';
		
		$page .= '</tr>';
	}

	$page .= '</tbody>';
	$page .= '</table>';
	
	if (addressbook_check_access($uid)) {
  
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyEdit',t('Family Edit'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'MemberAdd',t('Member Add'));
	
		if (variable_get('addressbook_map_link',0)==1) {
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'Map',t('Show Map'));
		}
	}
	
	$page .= ' | '.oostpoort_hiddenlink('AddressbookForm','','','FamilyList',t('Return'));
	
	$page .= '</div>';
	$page .= '</form>';
	
	return $page;
}

/**
 * Render a page showing the selected family in detail
 * @return HTML
 */
function addressbook_family_edit()  {
		
	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=htmlentities($_POST["fid"]);
	
	// *****************************
	// Query Database
	// *****************************
	
	$query = '
		SELECT 
			a.fid, 
			a.middle_name, 
			a.last_name, 
			a.street, 
			a.zipcode, 
			a.city, 
			a.country, 
			a.telephone, 
			a.last_updated, 
			a.uid,
			b.picture
		FROM 
			addressbook_family a LEFT JOIN addressbook_picture b ON b.fid=a.fid and b.mid=0
		WHERE 
			a.fid='.$fid;
					
	echo $query;
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Check Access
	// *****************************
	
	if ( !addressbook_check_access($data->uid) ) {
			drupal_access_denied();
			return;
	}
	
	// *****************************
	// Get posted Inputs
	// *****************************
	
	if ( isset($_POST["middle_name"]) ) {
		$middle_name=htmlentities($_POST['middle_name']);
	} else {
	   $middle_name=$data->middle_name;
	}
	
	if ( isset($_POST["last_name"]) ) {
		$last_name=htmlentities(ucfirst($_POST['last_name']));
	} else {
		$last_name=$data->last_name;
	}
	
	if ( isset($_POST["street"]) ) {
		$street=htmlentities(ucfirst($_POST['street']));
	} else {
		$street=$data->street;
	}
	
	if ( isset($_POST["zipcode"]) ) {
		$zipcode=htmlentities($_POST['zipcode']);
	} else {
		$zipcode=$data->zipcode;
	}
	
	if ( isset($_POST["city"]) ) {
		$city=htmlentities($_POST['city']);
	} else {
		$city=$data->city;
	}
	
	if ( isset($_POST["country"]) ) {
		$country=htmlentities($_POST['country']);
	} else {
		$country=$data->country;
	}
	if ($fid==0) {
		$country=variable_get('addressbook_country', 'Nederland');
	}
	
	if ( isset($_POST["telephone"]) ) {
		$telephone=htmlentities($_POST['telephone']);
	} else {
		$telephone=$data->telephone;
	}
	
	if ( isset($_POST["owner"]) ) {
		$owner=htmlentities($_POST['owner']);
	} else {
		$owner=$data->owner;
	}
			
	// *****************************
	// Create Page
	// *****************************

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';

	drupal_set_title(t('Addressbook family edit'));
	
	$page .= '<fieldset>' ;	
	$page .= '<legend>'.t('Family details').'</legend>';
	
	$page .= addressbook_load_picture($data->picture, 0, 0, "right", true);
	
	$page .= addressbook_form_item(
		t("Last Access"),
		addressbook_view_timestamp($data->last_updated),
		t("Last access date"), 
		true );
		
	$page .= addressbook_form_item(
		t("Owner"),
		addressbook_view_owner($owner,$readonly),
		t("Drupal owner (user) of address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Middle Name"),
		addressbook_view_text("middle_name", 10, 10, $middle_name, $readonly),
		t("Middle name of the family name (for example: van, de, etc..)"), 
		$readonly );

	$page .= addressbook_form_item(
		t("Last Name"),
		addressbook_view_text("last_name", 35, 35, $last_name, $readonly),
		t("Last name of the family name."), 
		$readonly );

	$page .= addressbook_form_item(
		t("Street"),
		addressbook_view_text("street", 50, 50, $street, $readonly),
		t("Last name of the family name."), 
		$readonly );

	$page .= addressbook_form_item(
		t("Zipcode"),
		addressbook_view_text("zipcode", 8, 8, $zipcode, $readonly),
		t("Zipcode of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("City"),
		addressbook_view_text("city", 50, 50, $city, $readonly),
		t("City of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Country"),
		addressbook_view_country($country, $readonly),
		t("Country of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Telephone"),
		addressbook_view_text("telephone", 16, 16, $telephone, $readonly),
		t("Telephone of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Image file"),
		addressbook_input_picture($filename, $readonly),
		t("Only jpg format is supported"), 
		$readonly );
					
	// Menu bar
	if ($fid==0) {
		$page .= addressbook_hiddenlink('AddressbookForm','',0,'FamilyInsert',t('Save'));
		if (addressbook_check_picture()) {
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','',0,'PictureDelete',t('Delete Picture'));
		}
		
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','',0,'FamilyList',t('Cancel'));
	} else {
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyUpdate',t('Save'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyDelete',t('Delete Family'));
		if (addressbook_check_picture()) {
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','',0,'PictureDelete',t('Delete Picture'));
		}
		
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Cancel'));
	}
	
	
	$page .= '</fieldset>';	
	
	$page .= '</div>';
	$page .= '</form>';
	
	return $page;
}


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
			first_name, 
			middle_name, 
			last_name, 
			mobile, 
			email, 
			mid, 
			fid, 
			uid, 
			(SELECT telephone FROM {addressbook_family} WHERE fid=addressbook_member.fid) telephone 
		FROM 
			{addressbook_member}
		WHERE 
			last_name LIKE "'.$parameter.'%" ';
  
	if ($role!='') {
		if ($_SESSION['role_type']=='active') {
			$query .= 'AND active_roles LIKE "%'.$role.'%" ';
		} else {
			$query .= 'AND wanted_roles LIKE "%'.$role.'%" ';
		}
	}
	$query .= 'ORDER BY last_name, middle_name ';
	$queryResult = db_query($query);

	// *****************************
	// View
	// *****************************

	drupal_set_title(t('Addressbook member list'));
  
	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
  
  	$page .= '<fieldset>' ;
	$page .= '<legend>'.t('Filteren').'</legend>';
	
	$page .= '<div class="form-item">';
	$page .= '<label>'.t('Achternaam').' :</label>';
	$page .= '<input id="text" name="Search" size="40" maxlength="40" type="text" value="'.$Search.'"> ';
	$page .= addressbook_hiddenlink('AddressbookForm','','','Search',t('Search'));
	$page .= '<div class="description">';
	$page .= t('Enter lastname and press enter to find the member.');
	$page .= '</div>';
	$page .= '</div>';
	
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

		if ( variable_get('addressbook_show_thumbnails',0)==1) {
			$query2  = 'SELECT pid, picture ';
			$query2 .= 'FROM {addressbook_picture} ';
			$query2 .= 'WHERE fid="'.$data->fid.'" AND mid="'.$data->mid.'"';
			$queryResult2 = db_query($query2);
			$tmp = db_fetch_object($queryResult2);
		
			$page_tmp .= '<td>';
			if ($tmp!=null) {
				// Show each picture with is found
				if ( addressbook_check_access($data->uid) ) {
					$page_tmp .= '<a href="'.url(URL_MEMBER_EDIT.'/').$data->mid.'">';
				} else {
					$page_tmp .= '<a href="'.url(URL_MEMBER_VIEW.'/').$data->mid.'">';
				}
				$filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
				$filename=str_replace(array("?q="), "", $filename);
				$page_tmp .='<img src="'.$filename.'" width=50 height=60/></a>';
			} 
			$page_tmp .= '</td>';
		}
     
		// Format Name
		$page_tmp .= '<td>';
		$name=addressbook_view_name($data->first_name,$data->middle_name,$data->last_name,true);
		$page_tmp .= oostpoort_hiddenlink('AddressbookForm', 'mid', $data->mid, 'MemberView', $name);
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
  
  return $page;
}

/**
 * Render a page showing the selected member in readonly mode
 * @return HTML
 */
function addressbook_member_view( $readonly=true )  {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=$_POST["mid"];
	if ($mid=="") {
		$mid=0;
	}
	addressbook_debug_param("mid", $mid);
	
	$fid=$_POST["fid"];
	if ($fid=="") {
		$fid=$_SESSION['fid'];
	}
	addressbook_debug_param("fid", $fid);
	
	// *****************************
	// Query Database
	// *****************************
	
	// Fetch member from database
	if ($mid==0) {
	
		$query  = '
			SELECT 
				b.street as street, 
				b.zipcode as zipcode, 
				b.city as city, 
				b.country as country, 
				b.telephone as telephone,
				b.last_name as last_name,
				b.middle_name as middle_name
			FROM 
				addressbook_family b
			WHERE 
				b.fid='.$fid;

	} else {
			
		$query  = '
			SELECT 
				a.first_name as first_name, 
				a.middle_name as middle_name, 
				a.last_name as last_name, 
				a.birth_day as birth_day, 
				a.mobile as mobile, 
				a.email as email, 
				a.work as work, 
				a.notes as notes,
				a.active_roles as active_roles, 
				a.wanted_roles as wanted_roles, 
				a.uid as uid, 
				a.fid as fid, 
				a.last_updated as last_updated,
				b.street as street, 
				b.zipcode as zipcode, 
				b.city as city, 
				b.country as country, 
				b.telephone as telephone
			FROM 
				addressbook_member a, addressbook_family b
			WHERE 
				b.fid=a.fid and
				a.mid='.$mid;
	}
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);
  
	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook member view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
	
	//addressbook_save_picture("fid",$fid);
				
	$page .= '<fieldset>' ;
	
	$page .= '<h4>'.$data->street.'</h4>';	
	$page .= '<h4>'.$data->zipcode.'&nbsp;&nbsp;'.$data->city.'</h4>';	
	$page .= '<h4>'.$data->country.'</h4>';	
	$page .= '<h4>'.$data->phone.'</h4>';	
	
	$page .= '</fieldset>' ;
		
	$page .= '<fieldset>' ;

	// Show this information only to admin users.
	if ( user_access('access administration pages') ) {
		
		if ($data->last_updated!="") {
			list($year, $time) = split(' ', $data->last_updated);
			list($access_year, $access_month, $access_day) = split('-', $year);
			list($access_hour, $access_minute, $access_sec) = split(':', $time);
			$last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;

			$page .= addressbook_form_item(
				t("Last updated:"),
				addressbook_view_text("last_updated", 22, 22, $last_updated, true),
				"", 
				true );
		}
		
		$page .= addressbook_form_item(
			t("Owner:"),
			addressbook_view_owner($data->uid, $readonly),
			t("Drupal owner of this member"), 
			$readonly );
	}

	// Query Database for Pictures
	/*$page .= '<td ROWSPAN=9 valign="top">';
	$query  = 'SELECT pid, picture ';
	$query .= 'FROM {addressbook_picture} WHERE fid="'.$data->fid.'" AND mid="'.$mid.'"';
	$queryResult = db_query($query);
	while ($data2 = db_fetch_object($queryResult)) {
		// Show each picture with is found
		$page .= '<a href="'.url(URL_PICTURE_VIEW.'/').$data2->pid.'">';
		$filename=url(THUMBNAILS_DIR.'/'.$data2->picture);
		$filename=str_replace(array("?q="), "", $filename);
		$page.='<img align="right" src="'.$filename.'"/>';
	}
	$page .= '</td>';*/
	
	$page .= addressbook_form_item(
					t("First Name:"),
					addressbook_view_text("first_name", 50, 50, $data->first_name, $readonly),
					t("First Name of member"), 
					$readonly );
				
	$page .= addressbook_form_item(
					t("Middle Name:"),
					addressbook_view_text("middle_name", 10, 10, $data->middle_name, $readonly),
					t("Middle Name of member"), 
					$readonly );
				
	$page .= addressbook_form_item(
					t("Last Name:"),
					addressbook_view_text("last_name", 50, 50, $data->last_name, $readonly),
					t("Last Name of member"), 
					$readonly );
						
	$page .= addressbook_form_item(
					t("Mobile:"),
					addressbook_view_text("mobile", 16, 16, $data->mobile, $readonly),
					t("Mobile number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Email:"),
					addressbook_view_text("email", 50, 50, $data->email, $readonly),
					t("Mobile number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Birthday:"),
					addressbook_view_birthday($data->birth_day, $readonly),
					t("Birtday of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Work:"),
					addressbook_view_textarea('work', 80, 4, 512, $data->work, $readonly),
					t("Work notes"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Notes:"),
					addressbook_view_textarea('notes', 80, 4, 512, $data->notes, $readonly),
					t("General notes"), 
					$readonly );

	$page .= addressbook_view_roles($active_roles,$wanted_roles, '1');

	$page .= '</fieldset>' ;

	// *****************************
	// Create Menu Bar
	// *****************************

	if ($readonly) {
		$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberEdit',t('Edit'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Back'));
	} else {
		if ($mid==0) {
			$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberInsert',t('Save'));
		} else {
			$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberUpdate',t('Save'));
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberDelete',t('Delete'));
		}
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberView',t('Cancel'));
	}
	
   /*if (variable_get('addressbook_map_link',0)==1) {
    $page .= '<td valign="top">';
    $page .= '<form action="'.url(URL_MAP_VIEW).'" method="POST">';
    $page .= '<input type="hidden" name="street" value="'.$data2->street.'" />';
    $page .= '<input type="hidden" name="city" value="'.$data2->city.'" />';
    $page .= '<input type="hidden" name="country" value="'.$data2->country.'" />';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="hidden" name="mid" value="'.$mid.'" />';
    $page .= '<input type="submit" value="'.t('Go to Map').'" />';
    $page .= '</form>';
    $page .= '</td>';
  }*/

	$page .= '</div>';
	$page .= '</form>';
 
	return $page;
}

function addressbook_map_view() {

  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook map view'));

  // Fetch search parameter out HTTP request
  $street=htmlentities($_POST['street']);
  $city=htmlentities($_POST['city']);
  $country=htmlentities($_POST['country']);
  $fid=htmlentities($_POST['fid']);
  $mid=htmlentities($_POST['mid']);

  drupal_set_html_head('');
  drupal_set_html_head('<script type=\'text/javascript\' src=\'http://api.map24.com/ajax/1.2/?init=default\'></script>');
  drupal_set_html_head('<script type=\'text/javascript\'>');
  drupal_set_html_head('var map = null;');

  drupal_set_html_head('function goMap24() {');
  drupal_set_html_head('var mrcContainer = new Map24.Webservices.Request.MapletRemoteControl(map);');

  drupal_set_html_head('map = Map24.Webservices.getMap24Application({');
  drupal_set_html_head('AppKey: "'.variable_get('addressbook_map_key', '').'",');
  drupal_set_html_head('MapArea: document.getElementById( "maparea" ),');
  drupal_set_html_head('Maptype: "JAVA",');
  drupal_set_html_head('MapWidth: 640,');
  drupal_set_html_head('MapHeight: 480,');
  drupal_set_html_head('MRC: mrcContainer');
  drupal_set_html_head('});');
  drupal_set_html_head('geocode(\''.$street.','.$city.','.$country.'\')');
  drupal_set_html_head('}');

  drupal_set_html_head('function geocode( addressString ){');
  drupal_set_html_head('map.Webservices.sendRequest(');
  drupal_set_html_head('new Map24.Webservices.Request.MapSearchFree(map, {');
  drupal_set_html_head('SearchText: addressString,');
  drupal_set_html_head('MaxNoOfAlternatives: 50');
  drupal_set_html_head('})');
  drupal_set_html_head(');');

  drupal_set_html_head('map.onMapSearchFree = function( event ){');
  drupal_set_html_head('var mrcContainer = new Map24.Webservices.Request.MapletRemoteControl( map );');
  drupal_set_html_head('var firstResult = event.Alternatives[0];');
  drupal_set_html_head('var lon = firstResult.Coordinate.Longitude;');
  drupal_set_html_head('var lat = firstResult.Coordinate.Latitude;');

  drupal_set_html_head('mrcContainer.push(');
  drupal_set_html_head('new Map24.Webservices.MRC.SetMapView({');
  drupal_set_html_head('Coordinates: new Map24.Coordinate( lon, lat ),');
  drupal_set_html_head('ClippingWidth: new Map24.Webservices.ClippingWidth(');
  drupal_set_html_head('{ MinimumWidth: 250 }');
  drupal_set_html_head(')');
  drupal_set_html_head('})');
  drupal_set_html_head(');');
  drupal_set_html_head('map.Webservices.sendRequest( mrcContainer );');
  drupal_set_html_head('}');
  drupal_set_html_head('}');

  drupal_set_html_head('</script>');

  $page.='<body onload=\'goMap24()\'>';

   // Show menu Bar
  $page .= '<br/>';
  $page .= '<table width="100%" border=0>';
  $page .= '<tr>';

  $page .= '<td valign="left">';
  $page .= '<b>'.t('Address: ').$street.', '.$city.', '.$country.'</b>';
  $page .= '</td>';

  $page .= '<td  valign="right">';
  $page.=' <input type="submit" value="'.t('Find Address').'" onclick="return geocode(\''.$street.','.$city.','.$country.'\')">';
  $page .= '</td>';

  $page .= '<td valign="right">';
  if ($mid=='') {
     $page .= '<form action="'.url(URL_FAMILY_VIEW.'/'.$fid).'" method="POST">';
  }
  else {
     $page .= '<form action="'.url(URL_MEMBER_VIEW.'/'.$mid).'" method="POST">';
     $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  }
  $page .= '<input type="submit" name="commit" value="'.t('Return').'" />';
  $page .= '</form>';
  $page .= '</td>';

  $page .= '</tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= '<div id="maparea"></div>';
  $page .= '</body>';

  $page .= '</div>';
  
  return $page;
}

// *********************************************************************************
// Database functions
// *********************************************************************************

/*
 * insert family SQL function
 * @return true of false
 */
function addressbook_family_insert() {

	// *****************************
	// Get posted Inputs
	// *****************************
		
	$middle_name=htmlentities($_POST['middle_name']);
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	$street=htmlentities(ucfirst($_POST['street']));
	$zipcode=htmlentities($_POST['zipcode']);
	$city=htmlentities(ucfirst($_POST['city']));
	$country=htmlentities($_POST['country']);
	$telephone=htmlentities($_POST['telephone']);
	$owner=htmlentities($_POST['owner']);
	
	// *****************************
	// Update Database
	// *****************************

	$query  = 'INSERT INTO addressbook_family (middle_name, last_name, street, zipcode, city, ';
	$query .= 'country, telephone, uid, last_updated) VALUES (';
	$query .= '"'.$middle_name.'",';
	$query .= '"'.$last_name.'",';
	$query .= '"'.$street.'",';
	$query .= '"'.$zipcode.'",';
	$query .= '"'.$city.'",';
	$query .= '"'.$country.'",';
	$query .= '"'.$telephone.'",';
	$query .= '"'.$owner.'",';
	$query .= 'SYSDATE() )';

	db_query($query);
	
	// Check new family entry.
	$query = '
	    select 
			fid 
		from 
			{addressbook_family} 
		where 
			zipcode like "'.$zipcode.'" and 
			street like "'.$street.'"';
			
	$queryResult = db_query($query);
	$tmp = db_fetch_object($queryResult);
	$fid = $tmp->fid;
	
	$_SESSION['fid']=$fid;
		
	// *****************************
	// create message
	// *****************************
	
	if ($fid!=0) {
		$msg .= t('Family').' '.$fid.' '.t('created!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	} 
		
	return 1;
}

/*
 * update family SQL function
 * @return true of false
 */
function addressbook_family_update() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=htmlentities($_POST["fid"]);
	
	$middle_name=htmlentities($_POST['middle_name']);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	
	$street=htmlentities(ucfirst($_POST['street']));
	
	$zipcode=htmlentities($_POST['zipcode']);
	
	$city=htmlentities(ucfirst($_POST['city']));
	
	$country=htmlentities($_POST['country']);
	if ($country=='') {
		$country=variable_get('addressbook_country', 'Nederland');
	}

	$telephone=htmlentities($_POST['telephone']);

	$owner=htmlentities($_POST['owner']);
	
	// *****************************
	// Update Database
	// *****************************
	
	$query = 'UPDATE addressbook_family SET ';
	$query .= 'middle_name = "'.$middle_name.'",';
	$query .= 'last_name = "'.$last_name.'",';
	$query .= 'street = "'.$street.'",';
	$query .= 'zipcode = "'.$zipcode.'",';
	$query .= 'city = "'.$city.'",';
	$query .= 'country = "'.$country.'",';
	$query .= 'telephone = "'.$telephone.'",';
	$query .= 'uid = "'.$owner.'",';
	$query .= 'last_updated = SYSDATE() ';
	$query .= 'WHERE fid='.$fid;

	$result = db_query($query);
	
	// *****************************
	// create message
	// *****************************
	
	$msg .= t('Family').' '.$fid.' '.t('updated!');
	drupal_set_message($msg, 'status');
	watchdog('user', $msg);
	
	return 0;
}

/*
 * delelete family SQL function
 * @return true of false
 */
function addressbook_family_delete() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=htmlentities($_POST["fid"]);
	
	// *****************************
	// Database actions
	// *****************************

	if ($fid!=0) {
	
		$query = 'delete FROM addressbook_family WHERE fid='.$fid;		
		db_query($query);  
	
		// *****************************
		// Create message
		// *****************************
	
		$msg .= t('Family').' '.$fid.' '.t('deleted!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	}
	
	return 1;		
}

/*
 * insert member SQL function
 * @return true of false
 */
function addressbook_insert_member() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=$_SESSION["fid"];
	addressbook_debug_param("fid", $fid);
	
	$first_name=htmlentities($_POST['first_name']);
	addressbook_debug_param("first_name", $last_name);
	
	$middle_name=htmlentities($_POST['middle_name']);
	addressbook_debug_param("middle_name", $middle_name);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	addressbook_debug_param("last_name", $last_name);
	
	$mobile=htmlentities($_POST['mobile']);
	addressbook_debug_param("mobile", $mobile);
	
	$email=htmlentities($_POST['email']);
	addressbook_debug_param("email", $email);
	
	$notes=htmlentities($_POST['notes']);
	addressbook_debug_param("notes", $notes);
	
	$work=htmlentities($_POST['work']);
	addressbook_debug_param("work", $work);
	
	$notes=htmlentities($_POST['notes']);
	addressbook_debug_param("notes", $notes);
	
	$owner=htmlentities($_POST['owner']);
	addressbook_debug_param("owner", $owner);
	
	$birthday=$_POST['birthday_day'].'-'.$_POST['birthday_month'].'-'.$_POST['birthday_year'];
	addressbook_debug_param("birthday", $birthday);
	
	// *****************************
	// Update Database
	// *****************************
	
	// Check if user allready exist. Protect against reload button
	$query  = 'select ';
	$query .= ' a.uid as uid ';
	$query .= 'from ';
	$query .= ' addressbook_member a ';
	$query .= 'where ' ;
	$query .= ' a.first_name="'.$first_name.'" and ';
	$query .= ' a.middle_name="'.$middle_name.'" and ';
	$query .= ' a.last_name="'.$last_name.'"';
	
	oostpoort_debug_sql($query);
	$queryResult = db_query($query);     
	$data = db_fetch_object($queryResult);
	
	if ($data->uid=="") {
	
	   // User does not exist, create entry 
		$query  = '
			INSERT INTO addressbook_member (
				first_name, 
				middle_name, 
				last_name, 
				birth_day, 
				mobile, 
				email, 
				notes, 
				work, 
				active_roles, 
				wanted_roles, 
				uid, 
				fid,
				last_updated) VALUES (';
		$query .= '"'.$first_name.'",';
		$query .= '"'.$middle_name.'",';
		$query .= '"'.$last_name.'",';
		$query .= '"'.addressbook_convert_date_reverse($birthday).'",';
		$query .= '"'.$mobile.'",';
		$query .= '"'.$email.'",';
		$query .= '"'.$notes.'",';
		$query .= '"'.$work.'",';
		$query .= '"'.$active_roles.'",';
		$query .= '"'.$wanted_roles.'",';
		$query .= '"'.$owner.'",';
		$query .= '"'.$fid.'",';
		$query .= 'SYSDATE() )';

		db_query($query);

		// *****************************
		// create message
		// *****************************
	
		$msg .= t('Member').' '.$mid.' '.t('is created in addressbook');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
	}
}

/*
 * update member SQl function
 * @return true of false
 */
function addressbook_update_member() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=htmlentities($_POST["mid"]);
	addressbook_debug_param("mid", $mid);
	
	$first_name=htmlentities($_POST['first_name']);
	addressbook_debug_param("first_name", $first_name);
	
	$middle_name=htmlentities($_POST['middle_name']);
	addressbook_debug_param("middle_name", $middle_name);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	addressbook_debug_param("last_name", $last_name);
	
	$mobile=htmlentities($_POST['mobile']);
	addressbook_debug_param("mobile", $mobile);
	
	$email=htmlentities($_POST['email']);
	addressbook_debug_param("email", $email);
	
	$notes=htmlentities($_POST['notes']);
	addressbook_debug_param("notes", $notes);
	
	$work=htmlentities($_POST['work']);
	addressbook_debug_param("work", $work);
	
	$notes=htmlentities($_POST['notes']);
	addressbook_debug_param("notes", $notes);
	
	$owner=htmlentities($_POST['owner']);
	addressbook_debug_param("owner", $owner);
	
	$birthday=$_POST['birthday_day'].'-'.$_POST['birthday_month'].'-'.$_POST['birthday_year'];
	addressbook_debug_param("birthday", $birthday);
	
	// *****************************
	// Update Database
	// *****************************
	
	$query = 'UPDATE {addressbook_member} SET ';
	$query .= 'first_name = "'.$first_name.'",';
	$query .= 'middle_name = "'.$middle_name.'",';
	$query .= 'last_name = "'.$last_name.'",';
	$query .= 'birth_day = "'.addressbook_convert_date_reverse($birthday).'",';
	$query .= 'mobile = "'.$mobile.'",';
	$query .= 'email = "'.$email.'",';
	$query .= 'notes = "'.$notes.'",';
	$query .= 'work = "'.$work.'",';
	$query .= 'uid = "'.$owner.'",';
	$query .= 'active_roles = "'.$active_roles.'",';
	$query .= 'wanted_roles = "'.$wanted_roles.'",';
	$query .= 'uid = "'.$owner.'",';
	$query .= 'last_updated = SYSDATE() WHERE mid='.$mid;

	db_query($query);
	addressbook_debug_sql($query);

	// *****************************
	// create message
	// *****************************
	
	$msg .= t('Member').' '.$mid.' '.t('is updated in addressbook');
	drupal_set_message($msg, 'status');
	watchdog('user', $msg);
}


/*
 * Delete member SQl function
 * @return true of false
 */
function addressbook_delete_member() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=htmlentities($_POST["mid"]);
	
	// *****************************
	// Database actions
	// *****************************

	if ($mid!=0) {
	
		$query = 'delete FROM addressbook_member WHERE mid='.$mid;		
		db_query($query);  
	
		// *****************************
		// Create message
		// *****************************
	
		$msg .= t('Member').' '.$mid.' '.t('deleted!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	}
	
	return 1;	
}


function addressbook_picture_delete() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=$_POST["fid"];
	if ($fid=="") $fid=0;
  
	// get fid from session scope
	$mid=$_POST["mid"];
	if ($mid=="") $mid=0;

	if ($fid!=0) {
	
		$query = 'delete FROM addressbook_picture WHERE fid='.$fid.' and mid='.$mid;	
		db_query($query);  
	
		// *****************************
		// Create message
		// *****************************
	
		$msg .= t('Picture deleted!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	}
	
}

// *********************************************************************************
// The end
// *********************************************************************************