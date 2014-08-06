<?php

require_once 'Utils.php';

// This class deals specifically with the BT Home Hub 5 and its logging in setup

class HomeHub5 extends Utils {

	// CHANGE THIS IP ADDRESS TO THE ADDRESS OF THE HOME HUB
	const URL = 'http://192.168.0.1';
	const BASE_PAGE = '/index.cgi';
	
	const ADVANCED_SETTINGS_PAGE = 9099;
	const LOGIN_PAGE = 9145;
	const HELPDESK_PAGE = 9140;
		
	/**
	*	Logging in consists of getting a cookie string that has a valid authentication object attached to it
	**/
	public function login($password) {		
		// Open a cookie file
		$cookie_file = tempnam ("/tmp", "CURLCOOKIE");
		
		// Send a GET request for a restricted page
		$get_output = $this->sendGet(self::URL.self::BASE_PAGE.'?active_page='.self::ADVANCED_SETTINGS_PAGE, array(), $cookie_file);
				
		// If we get a reply, process on
		if ($get_output) {					
			// Use a regex to get the auth_key out of the page
			// Could have parsed the DOM, but the HTML looked pretty ropey and this was the quickest way
			if (preg_match('/name="auth_key" value="(?<auth_key>\d*)"/si', $get_output['body'], $auth_match)) {
				$auth_key = $auth_match['auth_key'];
			} else {
				// If we can't get an auth key then give up
				return false;
			}
									
			// We have everything we need to attempt to make a POST request
			$md5_pass = strtolower(md5($password.$auth_key));

			// The POST fields
			$fields = array (
				'active_page' => self::LOGIN_PAGE,
				'mimic_button_field' => 'submit_button_login_submit: ..',
				'post_id' => 0,
				'md5_pass' => $md5_pass,
				'auth_key' => $auth_key
			);			
						
			// Make the POST request
			$post_output = $this->sendPost(self::URL.self::BASE_PAGE, $fields, array(), self::HTTP_REQUEST_STANDARD, $cookie_file);
						
			// If the POST completes successfully, then all we need to return is the cookie name which we'll use later
			if ($post_output['http_code'] == 302) {
				// We have a redirect, but that is not indicative of a successful login
				// Check to see if we're being redirected back to the page we originally requested
				$redirect_url = urldecode($post_output['info']['redirect_url']);
				$redirect_url_parts = parse_url($redirect_url);
				parse_str($redirect_url_parts['query'], $query_parts);

				if (isset($query_parts['active_page']) && $query_parts['active_page'] == self::ADVANCED_SETTINGS_PAGE) {
					// Everything is good, so return the filename of the cookie
					return $cookie_file;
				} else {
					return false;
				}
			}
		} else {
			// If we can't get any response from the hub, give up
			return false;
		}
		
		// If anything falls through then it failed
		return false;
	}
	
	/**
	* Get a specific page. Requires a page ID (index.cgi?active_page=XXXX) and the cookie file location as determined in $this->login();	
	* The returned value is excessively verbose
	**/
	public function getPage ($page_id, $cookie) {
		$get_output = $this->sendGet(self::URL.self::BASE_PAGE.'?active_page='.$page_id, array(), $cookie);
		return $get_output;
	}
}