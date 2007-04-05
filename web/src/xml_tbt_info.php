<?php

include_once("include/db.inc.php");
include_once("include/tbt-php.php");

$id = $_GET['id'];
$action = $_GET['action'];

switch ($action) {

	// fetch a unique tbt
	case "getbyid":
		$TBT = new TBT_DB;
		if($id != "") {
			if(($row = $TBT->get_by_id($id))==false) {
				$code="KO";
				$errmsg = $TBT->get_error();
			} else $code="OK";
			$num=1;
		}
		break;

	// get all the tbts related to the provided id
	case "related":
		$TBT = new TBT_DB;
		if($id != "") {
			if(($row = $TBT->get_related($id))==false) {
				$code="KO";
				$errmsg = $TBT->get_error();
			} else $code="OK";
			$num = sizeof($row) - 1;
		}
		break;

	default:
		$TBT = new TBT_DB;
			if(($row = $TBT->get_n(""))==false) {
				$code="KO";
				$errmsg = $TBT->get_error();
			} else $code="OK";
			$num = sizeof($row) - 1;
		break;
}



header("Content-Type: text/xml");
echo "<?xml version=\"1.0\"?>\n";

echo "<tbt_response>\n";
echo "<response_status>\n";
echo "\t<code>\n";
echo "$code\n";
echo "\t</code>\n";
echo "\t<errmsg>\n";
echo "$errmsg\n";
echo "\t</errmsg>\n";
echo "\t<num>\n";
echo "$num\n";
echo "\t</num>\n";
echo "</response_status>\n";

echo "\t<tbts>\n";

if($action == "getbyid" && $code=="OK") {

	echo "\t<tbt>\n";
	echo "\t\t<id>".$row["id"]."</id>\n";
	echo "\t\t<title>".$row["title"]."</title>\n";
	echo "\t\t<author>".$row["author"]."</author>\n";
	echo "\t\t<city>".$row["city"]."</city>\n";
	$TBT_JSRender = new TBT_JSRender($upload_dir."/".$row["file"]);
	echo "\t\t<data>".$TBT_JSRender->get_jsstr()."</data>\n";
	echo "\t\t<email>".$row["email"]."</email>\n";
	echo "\t\t<datetime>".$row["datetime"]."</datetime>\n";
	echo "\t\t<related_to>".$row["reaction_to"]."</related_to>\n";
	echo "\t</tbt>\n";

} else if (($action == "related" || $action =="" ) && $code=="OK") {

	foreach($row as $r) {

		if($r["id"]!="") {
			echo "\t<tbt>\n";
			echo "\t\t<id>".$r["id"]."</id>\n";
			echo "\t\t<title>".$r["title"]."</title>\n";
			echo "\t\t<author>".$r["author"]."</author>\n";
			echo "\t\t<city>".$r["city"]."</city>\n";
			$TBT_JSRender = new TBT_JSRender($upload_dir."/".$r["file"]);
			echo "\t\t<data>".$TBT_JSRender->get_jsstr()."</data>\n";
			echo "\t\t<email>".$r["email"]."</email>\n";
			echo "\t\t<datetime>".$r["datetime"]."</datetime>\n";
			echo "\t\t<related_to>".$r["reaction_to"]."</related_to>\n";
			echo "\t</tbt>\n";
		}

	}

}

echo "\t</tbts>\n";

echo "</tbt_response>\n";
?>
