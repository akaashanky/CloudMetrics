var cmApp = angular.module('cmApp', [
  "ngRoute",
  "mobile-angular-ui",
]).config(function($routeProvider) {
      $routeProvider.when('/', {        
      });
  });

//Load the Visualization API and the piechart package.
google.load('visualization', '1.0', {'packages':['corechart']});



cmApp.controller('KPIProfitabilityController', ['$scope', '$http', function($scope, $http) {
	$scope.getProfitabilityKpi = function() {
        $http({method: 'GET', url: '../../api/financial/kpi/profitability'}).
            success(function(data, status, headers, config) {
            	if(data.code == 1){
	               $scope.profitability = data.data;
	               //set the green and red movement tags
	               if($scope.profitability.momRevenueGrowth < 0){
	            	   $scope.isRevenueGrowthNegative = 'danger';
	               }else $scope.isRevenueGrowthNegative = 'success';
	               if($scope.profitability.momGrossProfitGrowth < 0){
	            	   $scope.isGrossProfitGrowthNegative = 'danger';
	               }else $scope.isGrossProfitGrowthNegative = 'success';
	               if($scope.profitability.momNetProfitGrowth < 0){
	            	   $scope.isNetProfitGrowthNegative = 'danger';
	               }else $scope.isNetProfitGrowthNegative = 'success';
	               
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
      data.addColumn('number','Revenue');
      data.addColumn('number','Net Profit');
      var noOfMonths = 0;
      var noOfMonthsOfNetProfit = $scope.profitability.monthlyNetProfitList.length;
      var noOfMonthsOfRevenue = $scope.profitability.monthlyRevenueList.length;
      if(noOfMonthsOfNetProfit < noOfMonthsOfRevenue){
    	  noOfMonths = noOfMonthsOfNetProfit;
      }else{
    	  noOfMonths = noOfMonthsOfRevenue;
      }
      data.addRows(noOfMonths);
      for(var i = 0; i < noOfMonths; i++){
    	  data.setCell(i, 0, "Month" + i);
    	  data.setCell(i, 1, $scope.profitability.monthlyRevenueList[i]);
    	  data.setCell(i, 2, $scope.profitability.monthlyNetProfitList[i]);
      }

      // Set chart options
      var options = {title: 'Revenue & Net Profit Trend', curveType: 'function', legend: { position: 'bottom' },
      		backgroundColor:'#252628', lineWidth:3,pointShape:'circle',pointSize:0,
      		chartArea: {width: '100%', height: '100%'},legend: {position: 'in'},titlePosition: 'in', axisTitlesPosition: 'in',
      		hAxis: {textPosition: 'in'}, vAxis: {textPosition: 'in'}, 'titleTextStyle':{'color':'#fff'},
      		colors:['#CBE86B','#E94E77'],curveType:'none',
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
    
    $scope.getProfitabilityKpi();
}]);
