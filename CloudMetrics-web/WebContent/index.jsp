<!DOCTYPE HTML>
<html manifest="" lang="en-US">
<head>
	<meta charset="UTF-8">
	<title>CMM</title>
	<style type="text/css">
		/**
         * Example of an initial loading indicator.
         * It is recommended to keep this as minimal as possible to provide instant feedback
         * while other resources are still being loaded for the first time
         */
		html, body {
			height: 100%;
			background-color: #1985D0
		}

		#appLoadingIndicator {
			position: absolute;
			top: 50%;
			margin-top: -15px;
			text-align: center;
			width: 100%;
			height: 30px;
			-webkit-animation-name: appLoadingIndicator;
			-webkit-animation-duration: 0.5s;
			-webkit-animation-iteration-count: infinite;
			-webkit-animation-direction: linear;
		}

		#appLoadingIndicator > * {
			background-color: #FFFFFF;
			display: inline-block;
			height: 30px;
			-webkit-border-radius: 15px;
			margin: 0 5px;
			width: 30px;
			opacity: 0.8;
		}

		@-webkit-keyframes appLoadingIndicator{
			0% {
				opacity: 0.8
			}
			50% {
				opacity: 0
			}
			100% {
				opacity: 0.8
			}
		}
	</style>
	<!-- The line below must be kept intact for Sencha Command to build your application -->
	<script type="text/javascript" src="/js/sencha-touch-all.js"></script>
	<script type="text/javascript" src="https://www.google.com/jsapi"></script>
	<script type="text/javascript">
		google.load('visualization', '1', {packages: ['corechart']});
	</script>
	<script type="text/javascript" src="/js/app.js"></script>

	<link href="/resources/css/app.css" rel="stylesheet" type="text/css"/>
	<link href="/resources/css/CMM.css" rel="stylesheet" type="text/css"/>

</head>
<body>
<div id="appLoadingIndicator">
	<div></div>
	<div></div>
	<div></div>
</div>
</body>
</html>
