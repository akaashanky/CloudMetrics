var testAngular = angular

.module('TestSection', ['ui.bootstrap'])

.config(['$routeProvider', function($routeProvider) {
	  $routeProvider

.when('/partial1', {templateUrl: '../test/partials/partial1.html', controller: TestSectionController })
	              .otherwise({redirectTo : '/partial1'});

	  //$locationProvider.html5Mode(true);
}]);	

function TestSectionController($scope){
	
}
