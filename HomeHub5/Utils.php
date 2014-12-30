<?php

class Utils {
    
    const HTTP_REQUEST_STANDARD = 'normal';
    const HTTP_REQUEST_JSON = 'json';
    
    public function sendGet($url, $headers = array(), $cookie_name = null) {
    	try {
    		$ch = curl_init();
    		
    		curl_setopt($ch, CURLOPT_URL, $url);
            curl_setopt($ch, CURLOPT_HEADER, 1);
			curl_setopt($ch, CURLINFO_HEADER_OUT, true);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_VERBOSE, 1);		
			
			if (!is_null($cookie_name)) {
				curl_setopt($ch, CURLOPT_COOKIEJAR, $cookie_name);
				curl_setopt($ch, CURLOPT_COOKIEFILE, $cookie_name);
			}
            
            if (!empty($headers)) {
                // The headers are in the format array ('name' => 'value')
				$header_array = array();
				foreach ($headers as $k => $v) {
					$header_array[] = $k.': '.$v;
				}
				
                curl_setopt($ch, CURLOPT_HTTPHEADER, $header_array);
            }
            
            $response = curl_exec($ch);
			$info = curl_getinfo($ch);
					
            $header_size = $info['header_size'];
			$request_headers = $info['request_header'];
			$http_code = $info['http_code'];

            $header = trim(substr($response, 0, $header_size));
            $body = substr($response, $header_size);
			
            return array (
				'request' => $request_headers,
				'info' => $info,
                'http_code' => $http_code,
                'headers' =>  $this->http_parse_headers($header),
                'body' => $body
            );
        } catch (Exception $ex) {
            return false;
        }
    }
    
    public function sendPost($url, $fields, $headers = array(), $type = self::HTTP_REQUEST_STANDARD, $cookie_name = null) {
        try {
            if (!is_array($fields)) {
                throw new Exception ('No variables supplied!');
            }
            
            if ($type == self::HTTP_REQUEST_JSON) {
                $post_string = json_encode($fields);
            } else {
                $fields_array = array();
                foreach ($fields as $k => $v) {
                    $fields_array[] = ($k).'='.($v);
                }
            
                $post_string = (implode('&', $fields_array));
            }
            
            $ch = curl_init();
            
            curl_setopt($ch, CURLOPT_URL, $url);
			curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $post_string);
			curl_setopt($ch, CURLINFO_HEADER_OUT, true);
            curl_setopt($ch, CURLOPT_HEADER, 1);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_VERBOSE, 1);
			
			if (!is_null($cookie_name)) {
				curl_setopt($ch, CURLOPT_COOKIEJAR, $cookie_name);
				curl_setopt($ch, CURLOPT_COOKIEFILE, $cookie_name);
			} else {
				curl_setopt($ch, CURLOPT_COOKIESESSION, true);
			}
            
            if (!empty($headers)) {
				// The headers are in the format array ('name' => 'value')
				$header_array = array();
				foreach ($headers as $k => $v) {
					$header_array[] = $k.': '.$v;
				}

                curl_setopt($ch, CURLOPT_HTTPHEADER, $header_array);
            }
            
            $response = curl_exec($ch);
			$info = curl_getinfo($ch);
					
            $header_size = $info['header_size'];
            $request_headers = array();
            if (isset($info['request_header'])) {
			     $request_headers = $info['request_header'];
            }
			$http_code = $info['http_code'];
						
            $header = trim(substr($response, 0, $header_size));
            $body = substr($response, $header_size);
            
            return array (
				'request' => $request_headers,
				'info' => $info,
				'post_string' => $post_string,
                'http_code' => $http_code,
                'headers' =>  $this->http_parse_headers($header),
                'body' => $body
				
            );
        } catch (Exception $ex) {
            return false;
        }
    }
    
    private function http_parse_headers( $header )
    {
        $retVal = array();
        $fields = explode("\r\n", preg_replace('/\x0D\x0A[\x09\x20]+/', ' ', $header));
        foreach( $fields as $field ) {
            if( preg_match('/([^:]+): (.+)/m', $field, $match) ) {
                $match[1] = preg_replace('/(?<=^|[\x09\x20\x2D])./e', 'strtoupper("\0")', strtolower(trim($match[1])));
                if( isset($retVal[$match[1]]) ) {
                    $retVal[$match[1]] = array($retVal[$match[1]], $match[2]);
                } else {
                    $retVal[$match[1]] = trim($match[2]);
                }
            }
        }
        return $retVal;
    }
    
    /**
     * Input a date in either text or timestamp form, and get a timestamp
     * 
     * @param unknown $date Either timestamp or date string
     * @return int Timestamp
     */
    public static function stringOrStamp($date) {
		// Get the current timestamp
		if ($date == 'now') {
			return time();
		}
    	if (is_numeric($date)) {
    		return $date;
    	} else {
    		return strtotime(str_replace('/', '-', $date));
    	}
    } 
    
    /**
     * Search a multidimensional array for an element and return the entry if found
     * 
     * @param string $element The element name to look for
     * @param string $value The value it should be
     * @param array $array The array to look in
     * @return array|boolean
     */
    public static function searchForElement($element, $value, $array) {
    	if (is_array($array)) {
    		foreach ($array as $key => $val) {
    			if (is_array($val)) {
	    			if (isset($val[$element]) && $val[$element] == $value) {
	    				return $array[$key];
	    			}
    			} else if (is_object($val)) {
    				if (isset($val->$element) && $val->$element == $value) {
    					return $array[$key];
    				}
    			}
    		}
    	}
    	return false;
    }
	
	/**
	 * Maps the $input from $in_min - $in_max to $out_min - $out_max
	 *
	 * @param int $input The input value
	 * @param int $in_min The minimum value $input will be
	 * @param int $in_max The maximum value $input will be
	 * @param int $out_min The mapped output value
	 * @param int $out_max The mapped output value
	 * @return int
	 */
	public static function map($input, $in_min, $in_max, $out_min, $out_max) {
		// Conform input incase it exceeds the boundaries
		if ($input < $in_min) {
			return $out_min;
		}
		if ($input > $in_max) {
			return $out_max;
		}
		
		return ($input - $in_min) * ($out_max - $out_min) / ($in_max - $in_min) + $out_min;
	}
}