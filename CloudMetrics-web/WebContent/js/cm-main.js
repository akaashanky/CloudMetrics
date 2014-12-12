var cmApp = angular.module('cmApp', [
  "ngRoute",
  "mobile-angular-ui",
]).config(function($routeProvider) {
      $routeProvider.when('/', {        
      });
  });

cmApp.controller('KPIPageController', ['$scope', '$http', function($scope, $http) {
	$scope.companyName = "My Company";
	
	$scope.getProfitabilityKpi = function() {
        $http({method: 'GET', url: '../../api/financial/kpi/profitability_test'}).
            success(function(data, status, headers, config) {
            	if(data.code == 1){
	                $scope.profitability = data.data;
	               
            	//Need to login
            	}else if(data.code == 2){
            		window.location.href = "signin.html";
	            }else{
	            	$scope.errorMessageFromServer = data.description;
	            }
            }).
            error(function(data, status, headers, config) {
                $scope.apps = data || "Request failed";
                $scope.status = status;
                $scope.view = './Partials/list.html';
            });
  };
	
	$scope.getProfitabilityKpi();
}]);



function validateFieldsOnSignUpForm(){

	var accountantEmail = $("#accountant_email").val();
	
	var atpos=accountantEmail.indexOf("@");
	var dotpos=accountantEmail.lastIndexOf(".");

	var pattern = new RegExp("/^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/");
	if(atpos<1 || dotpos<atpos+2 || dotpos+2 >= accountantEmail.length){
		$("#infoEmailTextMsg").hide();
		$("#accountantEmailInvalidFormat").show();
		$("#accountant_email").focus();
		$("#accountant_email_div").addClass("has-error");
		return false;
	}

	else {
		$("#accountantEmailInvalidFormat").hide();
		$("#accountant_email_div").removeClass("has-error");
		return true;
	}

}
