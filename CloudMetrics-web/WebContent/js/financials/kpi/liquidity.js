var cmApp = angular.module('cmApp', [
  "ngRoute",
  "mobile-angular-ui",
]).config(function($routeProvider) {
      $routeProvider.when('/', {        
      });
  });

//Load the Visualization API and the piechart package.
google.load('visualization', '1.0', {'packages':['corechart']});



cmApp.controller('KPILiquidityController', ['$scope', '$http', function($scope, $http) {

	$scope.getLiquidityKpi = function() {
        $http({method: 'GET', url: '../../api/financial/kpi/liquidity'}).
            success(function(data, status, headers, config) {
            	if(data.code == 1){
	               $scope.liquidity = data.data;
	               //set the green and red movement tags
	               if($scope.liquidity.momCashAndCashEqGrowth < 0){
	            	   $scope.isCashAndCashEqGrowthNegative = 'danger';
	               }else $scope.isCashAndCashEqGrowthNegative = 'success';
	               if($scope.liquidity.momCurrentRatioGrowth < 0){
	            	   $scope.isBankAccountGrowthNegative = 'danger';
	               }else $scope.isBankAccountGrowthNegative = 'success';
	               if($scope.liquidity.momBankAccountGrowth < 0){
	            	   $scope.isCurrentRatioGrowthNegative = 'danger';
	               }else $scope.isCurrentRatioGrowthNegative = 'success';
	               
	               $scope.drawChart();
            	//Need to login
            	}else if(data.code == 2){
            		window.location.href = "../user/login.html";
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
	

    // Callback that creates and populates a data table,
    // instantiates the pie chart, passes in the data and
    // draws it.
     $scope.drawChart = function() {

      // Create the data table.
      var data = new google.visualization.DataTable();
      data.addColumn('string', 'Month');
      data.addColumn('number','Cash In Hand');
      data.addColumn('number','Current Ratio');
      var noOfMonths = 0;
      var noOfMonthsOfCash = $scope.liquidity.monthlyCashAndCashEqList.length;
      var noOfMonthsOfCurrentRatio = $scope.liquidity.monthlyCurrentRatioList.length;
      if(noOfMonthsOfCash < noOfMonthsOfCurrentRatio){
    	  noOfMonths = noOfMonthsOfCash;
      }else{
    	  noOfMonths = noOfMonthsOfCurrentRatio;
      }
      data.addRows(noOfMonths);
      for(var i = 0; i < noOfMonths; i++){
    	  data.setCell(i, 0, "Month" + i);
    	  data.setCell(i, 1, $scope.liquidity.monthlyCashAndCashEqList[i]);
    	  data.setCell(i, 2, $scope.liquidity.monthlyCurrentRatioList[i]);
      }

      // Set chart options
      var options = {title: 'Liquidity Trend', curveType: 'function', legend: { position: 'bottom' },
      		backgroundColor:'#252628', lineWidth:3,pointShape:'circle',pointSize:0,
      		chartArea: {width: '100%', height: '100%'},legend: {position: 'in'},titlePosition: 'in', axisTitlesPosition: 'in',
      		hAxis: {textPosition: 'in'}, vAxis: {textPosition: 'in'}, 'titleTextStyle':{'color':'#fff'},
      		colors:['#CAD4F8','#E65165'],curveType:'none',
              vAxes: {
                  0: {
                      textPosition: 'none',
                      gridlines: {
                          color: 'transparent'
                      },
                      baselineColor: 'transparent'
                  },
                  1: {
                      gridlines: {
                          color: 'transparent'
                      }
                  }
              },
              hAxes: {
                  0: {
                      textPosition: 'none',
                      gridlines: {
                          color: 'transparent'
                      },
                      baselineColor: 'transparent'
                  },
                  1: {
                      gridlines: {
                          color: 'transparent'
                      }
                  }
              }};

      // Instantiate and draw our chart, passing in some options.
      var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
      chart.draw(data, options);
    };
    
    $scope.getLiquidityKpi();
}]);
