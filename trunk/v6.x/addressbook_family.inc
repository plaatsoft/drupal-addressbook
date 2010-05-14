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
 
// ************************************
// StateMachine
// ************************************
 
function family_list($PrsId=0) {

	echo var_dump($_POST);
	
	// *****************************
	// Process POST an SESSION variables
	// *****************************
	
	$Action=htmlentities($_POST['Action']);
	
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
			
		default: 
			$page = addressbook_family_list();
			break;
	}
	print theme("page", $page);
}
			
			
// ************************************
// Views
// ************************************

/*
 * Function shows all family in a list form
 * @return HTML
 */
function addressbook_family_list() {

	global $user;

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

	$Search=$_POST["Search"];

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
		$page_tmp .= oostpoort_hiddenlink('AddressbookForm', 'AdrId', $data->fid, 'FamilyView', $name);
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
 
	$page .= '</div>';
	$page .= '</form>';
	
	return $page;
}

  /*if ( user_access('add address')) {
    $page .= '<td ALIGN="right">';
    $page .= '<form action="'.url(URL_FAMILY_EDIT.'/0').'" method="POST">';
    $page .= '<input type="submit" value="'.t('New Family').'" />';
    $page .= '</form>';
    $page .= '</td>';
  }*/
  
/**
 * Render a page showing the selected family in detail
 * @return HTML
 */
function addressbook_family_view( )  {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$AdrId=$_POST["AdrId"];
	
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
			FID='.$AdrId;
			
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook family view') );

	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
	
	$page .= '<form enctype="multipart/form-data" name="OostpoortForm" method="POST" >';
	$page .= '<div class="oostpoort">';
	
	//addressbook_save_picture("Adr",$AdrId);
						
	$page .= '<fieldset>' ;
	$page .= '<legend></legend>';

	//$page .= addressbook_picture("Adr",$AdrId, 0, 0,"right", true);
	
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
			fid='.$AdrId.' 
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
		$name=view_name($data->first_name,$data->middle_name,$data->last_name,true);
		if ( check_access( $data->uid )) {
			$page .= l($name,URL_MEMBER_EDIT.'/'.$data->mid);
		} else {
			$page .= l($name,URL_MEMBER_VIEW.'/'.$data->mid);
		}
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
	
	if (check_access($uid)) {
  
		$page .= oostpoort_hiddenlink('AddressbookForm','AdrId',$AdrId,'EditFamily',t('Edit Family'));
		$page .= ' | '.oostpoort_hiddenlink('AddressbookForm','AdrId',$AdrId,'EditFamily',t('Add Member'));
		$page .= ' | '.oostpoort_hiddenlink('AddressbookForm','AdrId',$AdrId,'AddPicture',t('Add Picture'));
	
		if (variable_get('addressbook_map_link',0)==1) {
			$page .= ' | '.oostpoort_hiddenlink('AddressbookForm','AdrId',$AdrId,'Map',t('Map'));
		}
	}
	
	$page .= ' | '.oostpoort_hiddenlink('AddressbookForm','','','FamilyList',t('Return'));
	
	$page .= '</div>';
	$page .= '</form>';
	
	print theme("page", $page);
}

/**
 * Render a page showing the selected family in detail
 * @return HTML
 */
function family_edit($fid=0)  {

	global $user;
	$page = '<div class="addressbook">';

	if ($fid==0) {
		drupal_set_title(t('Addressbook create new family'));
	} else {
		drupal_set_title(t('Addressbook edit family'));
	}

	// Validate HTTP URL parameter
	if ( !check_number($fid) )
	{
		return hacker_warning();
	}

	// Save fid in session scope
	$_SESSION["fid"]=$fid;
  
	// Clear mid in session scope
	$_SESSION["mid"]=0;

	// Check access rights
	if ( $fid != 0 ) {
	
		// If not a new family is created, check entry access rights
		$query  = 'SELECT uid FROM {addressbook_family} WHERE fid='.$fid;
		$queryResult = db_query($query);
		$tmp = db_fetch_object($queryResult);

		if ( !check_access($tmp->uid) ) {
			// Deny access, no more clues.
			// Everything will be in the watchdog's URL for the administrator to check.
			drupal_access_denied();
			return;
		}
	}	

	// Fetch data from HTTP request
	$commit = isset($_POST['commit']) ? '1' : '0';
	$family_delete = isset($_POST['family_delete']) ? '1' : '0';
	$go_delete = isset($_POST['go_delete']) ? '1' : '0';

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

	// Delete are you sure form
	if (($family_delete=='1') && ($fid!='0') && ($go_delete=='0')) {
		$page .= '<br/>';
		$page .= t('Are you sure you want to delete this family?');
		$page .= '<br/>';
		$page .= '<br/>';
		$page .= '<table align="left">';
		$page .= '<tr><td>';
		$page .= '<form action="'.url(URL_FAMILY_EDIT.'/'.$fid).'" method="POST">';
		$page .= '<input type="submit" name="go_delete" value="'.t('Yes').'" />';
		$page .= '<input type="hidden" name="family_delete" />';
		$page .= '</form>';
		$page .= '</td><td>';
		$page .= '<form action="'.url(URL_FAMILY_EDIT.'/'.$fid).'" method="POST">';
		$page .= '<input type="submit" name="cancel_delete" value="'.t('No').'" />';
		$page .= '</form>';
		$page .= '</td></tr>';
		$page .= '</table>';
		$page .= '</div>';
		return $page;
	}

	// Delete selected address in database
	if (($commit=='0') && ($family_delete=='1') && ($fid!='0') && ($go_delete=='1')) {
		$query = 'DELETE FROM {addressbook_family} WHERE fid='.$fid;
		$result = db_query($query);
		if ($result==1) {
			// Query succesfull
			watchdog('user', 'Family '.$fid.' is deleted in the addressbook');
			return family_list($_SESSION["sort"]);
		} else {
			// Query error
			$page .= '<br/>';
			$page .= t('Failed to deleted the selected family, try again');
			$page .= '<br/>';
			$page .= '<br/>';
			$page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
			$page .= '<input type="submit" value="'.t('Return').'" />';
			$page .= '</form>';
			$page .= '</div>';
			return $page;
		}
	}

	// Insert new family in database
	if (($commit=='1') && ($family_delete=='0') && ($fid == '0')) {
		$error=0;

		// Valid input.
		if ($last_name=='') $error=1;
		if ($street=='') $error=1;
		if ($zipcode=='') $error=1;
		if ($city=='') $error=1;

		if ($error=='0') {
			if (($owner==0) && !user_access('access administration pages')) {
				// Set new owner because items did not found owner yet!)
				$owner=$user->uid;
			}

			if (insert_family($middle_name, $last_name, $street, $zipcode, $city, $country, $telephone, $owner)) {
         
				// Query succesfull
				return family_list($_SESSION["sort"]);
				
			} else {
				
				// Query error
				$page .= '<br/>';
				$page .= t('Failed to create a new family, try again!');
				$page .= '<br/>';
				$page .= '<br/>';
				$page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
				$page .= '<input type="submit" value="'.t('Return').'" />';
				$page .= '</form>';
				$page .= '</div>';
				return $page;
			}
		}
	}

	// Update address in database if input in valied
	if (($commit=='1') && ($family_delete=='0') && ($fid != '0')) {
		
		$error=0;

		// Valid input.
		if ($last_name=='') $error=1;
		if ($street=='') $error=1;
		if ($zipcode=='') $error=1;
		if ($city=='') $error=1;

		if ($error=='0') {

			if (($owner==0) && !user_access('access administration pages')) {
				// Set new owner because items did not found owner yet!)
				$owner=$user->uid;
			}
			if (update_family( $middle_name, $last_name, $street, $zipcode, $city, $country, $telephone, $owner, $fid)) {
				// Query succesfull
				return family_view2($fid);
			} else {
				// Query failed
				$page .= '<br/>';
				$page .= t('Failed to update the family, try again!');
				$page .= '<br/>';
				$page .= '<br/>';
				$page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
				$page .= '<input type="submit" value="'.t('Return').'" />';
				$page .= '</form>';
				$page .= '</div>';
				return $page;
			}
		}
	}

	// Fetch address from database
	if (($fid!='0') && ($commit=='0')) {
		$query  = 'SELECT fid, middle_name, last_name, street, zipcode, city, country, telephone, ';
		$query .= 'last_updated, uid FROM {addressbook_family} WHERE fid='.$fid;
		$queryResult = db_query($query);
		$tmp = db_fetch_object($queryResult);

		// If no data is found. Show anti hacking message
		if ($tmp==null) {
			return hacker_warning();
		}

		$middle_name=$tmp->middle_name;
		$last_name=$tmp->last_name;
		$street=$tmp->street;
		$zipcode=$tmp->zipcode;
		$city=$tmp->city;
		$country=$tmp->country;
		$telephone=$tmp->telephone;
		$owner=$tmp->uid;
		list($year, $time) = split(' ', $tmp->last_updated);
		list($access_year, $access_month, $access_day) = split('[/.-]', $year);
		list($access_hour, $access_minute, $access_sec) = split(':', $time);
		$last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;
	}

	// Show address information in readonly mode
	$page .= '<br/>';

	$page .= '<form action="'.url(URL_FAMILY_EDIT.'/').$fid.'" method="POST">';
	$page .= '<table cellpadding=2 >';

   if ( user_access('access administration pages') ) {
      // Only administrator can overrule owner information.
     $page .= view_owner($owner,'0');
  }
  else {
     $page .= view_owner($owner,'1');
  }

  $page .= view_line(t('Last updated'),'<input id="text" name="last_updated" size="22" type="text" value="'.$last_updated.'" READONLY />');

  $page .= '<tr><td valign="top">';
  $page .= t('Middle Name');
  $page .= '</td><td>';
  $page .= '<input id="text" name="middle_name" size="10" maxlength="10" type="text" value="'.$middle_name.'"/>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Last Name*');
  $page .= '</td><td>';
  $page .= '<input id="text" name="last_name" size="35" maxlength="35" type="text" value="'.$last_name.'" ';
  if (($last_name=='') && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Street*');
  $page .= '</td><td>';
  $page .= '<input id="text" name="street" size="50" maxlength="50" type="text" value="'.$street.'" ';
  if (($street=='') && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Zipcode*');
  $page .= '</td><td>';
  $page .= '<input id="text" name="zipcode" size="8" maxlength="8" type="text" value="'.$zipcode.'" ';
  if (($zipcode=='') && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('City*');
  $page .= '</td><td>';
  $page .= '<input id="text" name="city" size="50" maxlength="50" type="text" value="'.$city.'" ';
  if (($city=='') && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  $page .= '</td></tr>';

  $page .= view_country($country, '0');

  $page .= '<tr><td valign="top">';
  $page .= t('Telephone');
  $page .= '</td><td>';
  $page .= '<input id="text" name="telephone" size="16" maxlength="16" type="text" value="'.$telephone.'" />';
  $page .= '</td></tr>';

  $page .= '</table>';

  $page .= '<br/>';
  $page .= t('* Marked fields are mandatory!').'<br/>';

   // Show menu Bar
  $page .= '<br/>';
  $page .= '<table align="left">';
  $page .= '<tr>';

  $page .= '<td valign="top">';
  $page .= '<tr><td valign="top">';
  $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  $page .= '<input type="submit" name="commit" value="'.t('Commit').'" />';
  $page .= '</form>';
  $page .= '</td>';

  if ($fid!='0') {
    // Only existing families can be deleted
    $page .= '<td valign="top">';
    $page .= '<form action="'.url(URL_FAMILY_EDIT.'/').$fid.'" method="POST">';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="submit" name="family_delete" value="'.t('Delete').'" />';
    $page .= '</form>';
    $page .= '</td>';
  }

  // Cancel new created address
  $page .= '<td valign="top">';
  if ($fid!='0') {
     $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
  } else
  {
     $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
  }
  $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  $page .= '<input type="submit" value="'.t('Cancel').'" />';
  $page .= '</form>';
  $page .= '</td>';

  $page .= '</tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= '<br/>';

  $page .= '</div>';
  print theme("page", $page);
}

// ************************************
// Database functions
// ************************************

/*
 * insert family SQL function
 * @return true of false
 */
function insert_family(
		$middle_name, 
		$last_name, 
		$street, 
		$zipcode, 
		$city, 
		$country, 
		$telephone, 
		$owner) {

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

	$result = db_query($query);
	if ($result==1) {
		// Query succesfull
		watchdog('user', 'A family is created in the addressbook');
		return true;
	} else {
		// Query failed
		return false;
	}
}

/*
 * update family SQL function
 * @return true of false
 */
function update_family( 
		$middle_name, 
		$last_name, 
		$street, 
		$zipcode, 
		$city, 
		$country, 
		$telephone, 
		$owner, 
		$fid) {

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
	if ($result==1) {
		// Query succesfull
		watchdog('user', 'Family '.$fid.' is updated in addressbook');
		return true;
		
	} else {
		// Query failed
		return false;
	}
}

// ************************************
// The end
// ************************************