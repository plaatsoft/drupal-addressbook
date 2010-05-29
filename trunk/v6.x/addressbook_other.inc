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
 
/**
 * Render a page showing the picture full screen.
 * @return HTML
 */
function addressbook_picture_view( $pid=0 ) {

  global $user;
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook view picture') );

  // Validate HTTP URL parameter
  if ( !check_number($pid) )
  {
     return hacker_warning();
  }

  // get fid from session scope
  $fid=$_SESSION["fid"];
  // get fid from session scope
  $mid=$_SESSION["mid"];

  $picture_delete = isset($_POST['picture_delete']) ? '1' : '0';
  $go_delete = isset($_POST['go_delete']) ? '1' : '0';

  // Check access rights
  $query  = 'SELECT uid, picture FROM {addressbook_picture} WHERE pid='.$pid;
  $queryResult = db_query($query);
  $tmp = db_fetch_object($queryResult);

  // If no data has been found. Show anti hacking message
  if ($tmp==null) {
    return hacker_warning();
  }

  // Delete 'Are you sure?' form
  if (($picture_delete=='1') && ($pid!='0') && ($go_delete=='0')) {
    $page .= '<br/>';
    $page .= t('Are you sure you want to delete the picture?');
    $page .= '<br/>';
    $page .= '<br/>';
    $page .= '<table align="left">';
    $page .= '<tr><td>';
    $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
    $page .= '<input type="submit" name="go_delete" value="'.t('Yes').'" />';
    $page .= '<input type="hidden" name="picture_delete" />';
    $page .= '</form>';
    $page .= '</td><td>';
    $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
    $page .= '<input type="submit" name="cancel_delete" value="'.t('No').'" />';
    $page .= '</form>';
    $page .= '</td></tr>';
    $page .= '</table>';
    $page .= '</div>';
    return $page;
  }

  // Delete selected picture from disk and database
  if (($picture_delete=='1') && ($pid!='0') && ($go_delete=='1')) {

     if (is_file(IMAGE_DIR.'/'.$tmp->picture)) {
       unlink(IMAGE_DIR.'/'.$tmp->picture);
     }
     if (is_file(THUMBNAILS_DIR.'/'.$tmp->picture)) {
       unlink(THUMBNAILS_DIR.'/'.$tmp->picture);
     }

     $query = 'DELETE FROM {addressbook_picture} WHERE pid='.$pid;
     $result = db_query($query);
     if ($result==1)
     {
       // Query succesfull
       watchdog('user', 'Picture '.$pid.' is deleted from the addressbook');

       $page .= '<td valign="top">';
       $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
       $page .= '<input type="submit" name="picture_delete" value="'.t('Delete').'" />';
       $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
       $page .= '</form>';
       $page .= '</td>';

       if ($mid!='0') {
         if (check_access($tmp->uid)) {
           return member_edit($mid);
         }
         else {
           return member_view($mid);
         }
       }
       else {
          return family_view2($fid);
       }
     }
     else {
       // Query failed
       $page .= '<br/>';
       $page .= t('Failed to delete picture, please try again');
       $page .= '<br/>';
       $page .= '<br/>';
       if ($mid!='0') {
         if (check_access($tmp->uid)) {
           $page .= '<form action="'.url(URL_MEMBER_EDIT.'/'.$mid).'" method="POST">';
         }
         else {
           $page .= '<form action="'.url(URL_MEMBER_VIEW.'/'.$mid).'" method="POST">';
         }
       }
       else {
          $page .= '<form action="'.url(URL_FAMILY_VIEW.'/'.$fid).'" method="POST">';
       }

       $page .= '<input type="submit" value="'.t('Return').'" />';
       $page .= '</form>';

       $page .= '</div>';
       return $page;
     }
  }

  // Show menu Bar
  $page .= '<br/>';
  $page .= '<table align="left">';
  $page .= '<tr>';

  if ( check_access($tmp->uid) ) {
    $page .= '<td valign="top">';
    $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
    $page .= '<input type="submit" name="picture_delete" value="'.t('Delete').'" />';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '</form>';
    $page .= '</td>';
  }

  $page .= '<td valign="top">';

  if ($mid==0)
  {
     $page .= '<form action="'.url(URL_FAMILY_VIEW.'/'.$fid).'" method="POST">';
  }
  else
  {
     if (check_access($tmp->uid)) {
        $page .= '<form action="'.url(URL_MEMBER_EDIT.'/'.$mid).'" method="POST">';
     }
     else {
        $page .= '<form action="'.url(URL_MEMBER_VIEW.'/'.$mid).'" method="POST">';
     }
  }
  $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  $page .= '<input type="submit" value="'.t('Return').'" />';
  $page .= '</form>';
  $page .= '</td>';
  $page .= '</tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= '<br/>';

  if ($picture_delete=='0') {
    $filename=url(IMAGE_DIR.'/'.$tmp->picture);
    $filename=str_replace(array("?q="), "", $filename);
    $page.='<img src="'.$filename.'"/>';
  }
  $page .= '<br/>';

  $page .= '</div>';
  print theme("page", $page);
}

/**
 * Render a page for uploading a picture.
 * @return HTML
 */
function addressbook_picture_edit( $mid=0 ) {

  global $user;
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook edit picture') );

  // Validate HTTP URL parameter
  if ( !check_number($mid) )
  {
     return hacker_warning();
  }

  //
  // Fetch data from HTTP request
  //
  $fid=$_POST['fid'];
  if ( !check_number($fid) )
  {
     return hacker_warning();
  }

  // Check access rights
  $query  = 'SELECT uid FROM {addressbook_family} WHERE fid='.$fid;
  $queryResult = db_query($query);
  $tmp = db_fetch_object($queryResult);

  // f no data has been found. Show anti hacking message
  if ($tmp==null) {
    return hacker_warning();
  }

  if ( !check_access($tmp->uid) ) {
    // Deny access, no more clues.
    // Everything will be in the watchdog's URL for the administrator to check.
    drupal_access_denied();
    return;
  }

  // Fetch Image upload in a HTTP request
  if ($_FILES['uploadedfile']['name']!='') {
    if ( (strstr($_FILES['uploadedfile']['name'],'.jpg')) || (strstr( $_FILES['uploadedfile']['name'],'.JPG')) )
    {
      $error='0';
      if(!is_dir(IMAGE_DIR))
      {
        if( !mkdir(IMAGE_DIR) )
        {
          $page .= '<br/>';
          $page .= t('Creating image directory failed!');
          $page .= '<br/>';
          $error='1';
        }
      }
      $filename= date('YmdHis', time()).'.jpg';

       // Check dimensions of image
      $dimensions = getimagesize($_FILES['uploadedfile']['tmp_name']);
      $width  = $dimensions[0];
      $height = $dimensions[1];
      $resize = 0;

      // Limit picture 
      if ($width>$height)
      {
         $ratio = 800 / $width;
         if ($ratio < 1)
         {
            $width=800;
            $height=$height*$ratio;
            $resize = 1;
         }
      }
      else {
         $ratio = 600 / $width;
         if ($ratio < 1)
         {
            $width=600;
            $height=$height*$ratio;
            $resize = 1;
         }
      }

      $page .= '<br/>';

      // Resize uploaded picture
      if ( $resize == 1 )
      {
         if (!image_scale($_FILES['uploadedfile']['tmp_name'], file_create_path(IMAGE_DIR.'/'.$filename), $width, $height)) {
              $page .= '<br>';
              $page .= t('Unable to create image. Is Gd library active in apache php module?');
              $error='1';
              $page .= '<br>';
         }
         else
         {
            chmod(IMAGE_DIR.'/'.$filename, 0666);
         }

      }
      else
      {
        // No resize needed, move file to storage 
        copy($_FILES['uploadedfile']['tmp_name'],IMAGE_DIR.'/'.$filename);
      }

      // Create thumbnails directory store
      if(!is_dir(THUMBNAILS_DIR))
      {
        if( !mkdir(THUMBNAILS_DIR) )
        {
          $page .= '<br/>';
          $page .= t('Creating image directory failed!');
          $page .= '<br/>';
          $error='1';
        }
      }

      $page .= '<br/>';

      $resize = 0;

      // Limit picture size on screen
      if ($width>$height)
      {
         $ratio  = 160 / $width;
         if ($ratio < 1)
         {
            $width=160;
            $height=$height*$ratio;
            $resize = 1;
         }
      }
      else
      {
         $ratio  = 120 / $width;
         if ($ratio < 1)
         {
            $width=120;
            $height=$height*$ratio;
            $resize = 1;
         }
      }

      // Create thumbnail picture
      // Resize uploaded picture
      if ( $resize == 1 )
      {
        if (!image_scale(IMAGE_DIR.'/'.$filename, file_create_path(THUMBNAILS_DIR.'/'.$filename), $width, $height)) {
          $page .= '<br>';
          $page .= t('Unable to create thumbnails image. Is the GD library active in the Apache PHP module?');
          $error='1';
          $page .= '<br>';
        }
        else
        {
            chmod(THUMBNAILS_DIR.'/'.$filename, 0666);
        }
      }
      else
      {
        // No resize needed, move file to storage 
        copy(IMAGE_DIR.'/'.$filename, THUMBNAILS_DIR.'/'.$filename);
      }

      if ($error=='0') {
        // Save picture in database
        $query  = 'INSERT INTO addressbook_picture (fid,mid,picture,uid,last_updated) VALUES (';
        $query .= '"'.$fid.'",';
        $query .= '"'.$mid.'",';
        $query .= '"'.$filename.'",';
        $query .= '"'.$user->uid.'",';
        $query .= 'SYSDATE() )';
        $result = db_query($query);
        if ($result==1)
        {
           // Query Succesfull
           watchdog('user', 'A picture has been added to address '.$id.' in addressbook');

           if ($mid=='0')
           {
              // Return to family view page
              return family_view2($fid);
           }
           else {
              return member_edit($mid);
           }
        }
        else {
          // Query Failed
          $page .= '<br/>';
          $page .= t('Failed to add picture, please try again!');
          $page .= '<br/>';
        }
      }
    }
    else {
      $page .= '<br/>';
      $page.=t('Only the jpg image format is supported for pictures');
      $page .= '<br/>';
      $_FILES['uploadedfile']['name']='';
    }
  }

  if ($_FILES['uploadedfile']['name']=='') {
    $page .= '<form enctype="multipart/form-data" action="'.url(URL_PICTURE_EDIT.'/').$mid.'" method="POST">';
    $page .= '<br/>';
    $page .= '<input type="hidden" />';
    $page .= t('Choose a file to upload:').' <input name="uploadedfile" type="file" />';

    $page .= '<br/>';
    $page .= '<div id="mission">';
    $page .= t('Warning: Only the jpg picture format is supported.');
    $page .= '<br/>';
    $page .= t('Image size must not exceed ').ini_get('upload_max_filesize').'B';
    $page .= '<br/></div>';
    $page .= '<br/>';

    // Show menu Bar
    $page .= '<table>';
    $page .= '<tr><td>';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="submit" value="'.t('Upload File').'" />';
    $page .= '</form>';
    $page .= '</td><td>';

    if ($mid=='0')
    {
      // Return to family view page
      $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
      $page .= '<input type="submit" value="'.t('Cancel').'" />';
      $page .= '</form>';
    }
    else {
      // Return to member view page
      $page .= '<form action="'.url(URL_MEMBER_EDIT.'/').$mid.'" method="POST">';
      $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
      $page .= '<input type="submit" value="'.t('Cancel').'" />';
      $page .= '</form>';
    }
    $page .= '</td></tr>';
    $page .= '</table>';
  }
  else
  {
    $page .= '<br/>';
    if ($mid=='0')
    {
      // Return to family view page
      $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
      $page .= '<input type="submit" value="'.t('Return').'" />';
      $page .= '</form>';
    }
    else {
      // Return to member view page
      $page .= '<form action="'.url(URL_MEMBER_EDIT.'/').$mid.'" method="POST">';
      $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
      $page .= '<input type="submit" value="'.t('Return').'" />';
      $page .= '</form>';
    }
  }

  $page .= '</div>';
  print theme("page", $page);
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
  print theme("page", $page);
}