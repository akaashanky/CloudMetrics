package com.cloudmetrics.util;

public class EnumCollection {
	
	public enum Environment {
		Prod, Dev
	}
	
	public enum BooleanEnum {
		Y, N
	}
	
	public enum EventStatus {
		failure(0),success(1),nosession(2);		
		private Integer value;
		
		private EventStatus(Integer value) {
			this.value = value;
		}		
		public Integer getValue() {
			return value;
		}
	}
}