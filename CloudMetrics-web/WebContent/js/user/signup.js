var cmApp = angular.module('cmApp', [
  "ngRoute",
  "mobile-angular-ui",
]).config(function($routeProvider) {
      $routeProvider.when('/', {        
      });
  });


  cmApp.controller('SignUpController', ['$scope', '$http', function($scope, $http) {
        $scope.isHidden = 'hidden';
		$scope.createAccount = function() {
			$http({method: 'POST',
				   url: '../../login',
				   data: "email="+$scope.user.EmailId + "&password="+$scope.user.Password + "&action=choosePassword",
				   headers: {'Content-Type': 'application/x-www-form-urlencoded'}}).
	            success(function(data, status, headers, config) {	            	
	                window.location.href = "../kpi/profitability.html";
	            }).
	            error(function(data, status, headers, config) {
	            	$scope.errorMessageFromServer = data.description;
	            	$scope.isHidden = 'show';
	            });
	  };
	  $scope.login = function(){
			 $http({
		        	method: 'POST', 
		        	url: '../../login', 
		        	data: "email="+$scope.user.EmailId + "&password="+$scope.user.Password + "&action=login",
		        	headers : { 'Content-Type': 'application/x-www-form-urlencoded'}}).
		            success(function(data, status, headers, config) {
		            	window.location.href = "../kpi/profitability.html";
		            }).
		            error(function(data, status, headers, config) {
		            	$scope.errorMessageFromServer = data.description;
		            	$scope.isHidden = 'show';
		            });
		};
	    
	}]);
