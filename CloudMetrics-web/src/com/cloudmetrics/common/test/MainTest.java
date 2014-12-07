package com.cloudmetrics.common.test;

import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

public class MainTest {
  public static void main(String g[]){
	  try 
	  {
	      SecureRandom randomGenerator = SecureRandom.getInstance("SHA1PRNG");         
	      System.out.println(randomGenerator.nextInt(9999));

	  } catch (NoSuchAlgorithmException nsae) {
	    // Forward to handler
	  }
  }
}
