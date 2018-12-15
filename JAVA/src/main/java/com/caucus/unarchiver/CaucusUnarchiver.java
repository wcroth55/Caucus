package com.caucus.unarchiver;

import java.util.*;
import java.io.*;
import java.sql.*;
import org.jdom.*;
import org.jdom.input.*;
import com.caucus.utils.*;

// Copyright (C) 2007 CaucusCare.com.  All rights reserved.
// This program is published under the terms of the GPL; see
// the file license.txt in the same directory.

public class CaucusUnarchiver {
	private Connection connection;
	
	public CaucusUnarchiver (Connection conn) {
		connection = conn;
	}
	
	//---Usage:
	//   java CaucusUnarchiver caucusHomeDir xmlDataDir confKey restoreGrades [-f fileContainingListOfXmlFiles ]
	//
	//   (If no -f argument, then reads list of XML files from stdin).

	public static void main(String[] args) {
		String line;
		BufferedReader in;
		
		//FileUtils.copyDirContentsToDir (new File("/home/roth/test1"), new File("/home/roth/test2"), false);
		
		if (args.length < 4)  System.exit(1);
		String caucusHomeDir = args[0];
		String xmlDataDir = args[1];
		String confKey = args[2];
		boolean restoreGrades = args[3].equals("1");
		
		if (args.length >= 6) {
			in = IOUtils.makeReaderFromFileName(args[5]);
		}
		else {
			InputStreamReader cvt = new InputStreamReader(System.in);
			in = new BufferedReader(cvt);
		}
		
		Map<String, String> parameters = IOUtils.loadConfigurationParametersFromFile(args[0] + "/SWEB/swebd.conf");
		Connection connection = getMysqlConnectionFromCaucusConnectString ((String) parameters.get("ConnectString"));
		
		CaucusUnarchiver unarchiver = new CaucusUnarchiver (connection);
		
		Map<String,String> itemLabels = new HashMap<String,String>();
		Map<String,String> responseTypes = makeResponseTypes();

		try {
			while ((line = in.readLine()) != null) {
				unarchiver.processXmlFile(line, caucusHomeDir, xmlDataDir, confKey, itemLabels,
						responseTypes, restoreGrades);
			}
		}
		catch (Exception e) {
		}
		System.out.println ("Done!");
	}
	
	protected static Map<String,String> makeResponseTypes() {
		Map<String,String> responseTypes = new HashMap<String,String>();
		responseTypes.put("wordwrap", "0");
		responseTypes.put("literal",  "1");
		responseTypes.put("html",     "2");
		responseTypes.put("cml",      "3");
		return responseTypes;
	}
	
	protected void processXmlFile(String xmlFileName, String caucusHomeDir,
			String xmlDataDir, String confKey, Map<String,String> itemLabels,
			Map<String,String> responseTypes, boolean restoreGrades) {
		
		System.out.println ("Processing " + xmlFileName);
		Element root = loadXMLFile(xmlDataDir + "/" + xmlFileName);
		if (root == null) {
			System.out.println ("Null root for " + xmlFileName);
			return;
		}
		Namespace caucus = root.getNamespace();
		
		if (root.getName().equals("item")) {
			System.out.println ("Item " + itemLabels);
			String itemKey = createItem (root, itemLabels);
			addItemToConference(itemKey, root, confKey, itemLabels);
			Map<String,Element> responses = getResponses (itemKey, root);
			addResponsesToItem (responses, itemKey, root, responseTypes);
			addQuizToItem (itemKey, root);
			copyAttachedFiles(xmlDataDir + "/items", root, caucusHomeDir, "ITEMS/" + itemKey);
		}
		
		else if (root.getName().equals("variables")) {
			List<Element> vlist = root.getChildren("variable", caucus);
			for (Iterator<Element> v = vlist.iterator();   v.hasNext();) {
				Element variable = (Element) v.next();
				InsertQuery query = new InsertQuery ("REPLACE INTO site_data", connection);
				query.addNum("cnum", confKey);
				query.addStr("name",  variable.getAttributeValue("name"));
				query.addStr("value", mapE000charsTo0000(variable.getText()));
				query.execute();
			}
		}
		
		else if (root.getName().equals("assignment")) {
			String assignKey = addAssignmentToConference (root, confKey);
			copyAttachedFiles(xmlDataDir + "/assignments", root, caucusHomeDir, "ASSIGN/" + assignKey);
		}
		
		else if (root.getName().equals("grades")) {
			String taskKey = addGradeTaskToConference (root, confKey);
			if (restoreGrades)  addGradeScoresToConference (root, confKey, taskKey);
		}
	}

	protected static String mapE000charsTo0000 (String text) {
		char [] chars = text.toCharArray();
		for (int i=0;   i<chars.length;   ++i) {
			if (chars[i] >= 0xE0000  &&  chars[i] < 0xE020) chars[i] -= 0xE0000;
		}
		return new String(chars);
	}
	
	protected void addGradeScoresToConference (Element root, String confKey, String taskKey) {
		Namespace caucus = root.getNamespace();
		
		List<Element> students = root.getChildren("student", caucus);
      for (Iterator<Element> s = students.iterator();   s.hasNext();) {
         Element student = (Element) s.next();
         
         InsertQuery query = new InsertQuery ("INSERT INTO grade_scores", connection);
			query.addNum("taskkey",    taskKey);
			query.addStr("userid",     student.getChildText("userid", caucus));
			query.addNum("scorenumb",  student.getChildText("scorenumb", caucus));
			query.addStr("scoretext",  student.getChildText("scoretext", caucus));
			query.addStr("done",       student.getChildText("done", caucus));
			query.addStr("comment",    student.getChildText("comment", caucus));
			
			System.out.println ("grades query=" + query);
			query.execute();
      }
	}
	
	protected String addGradeTaskToConference (Element root, String confKey) {
		Namespace caucus = root.getNamespace();
		
		InsertQuery query = new InsertQuery("INSERT INTO grade_tasks", connection);
		query.addStr ("taskkey",  null);
		query.addNum ("cnum",     confKey);
		query.addStr ("class",    root.getChildText("class", caucus));
		query.addStr ("title",    root.getChildText("title", caucus));
		query.addStr ("type",     root.getChildText("type", caucus));
		query.addStr ("due",      root.getChildText("due", caucus));
		query.addNum ("points",   root.getChildText("points", caucus));
		query.addNum ("weight",   root.getChildText("weight", caucus));
		query.addStr ("comment",  root.getChildText("comment", caucus));
		
		String added = root.getChildText("added", caucus);
		if (added.equals(""))  added = "0000-00-00";
		query.addStr("added", added);
		
		System.out.println ("add task = " + query.toString());
		return query.execute();
	}
	
	protected String addAssignmentToConference (Element root, String confKey) {
		Namespace caucus = root.getNamespace();
		
		InsertQuery query = new InsertQuery("INSERT INTO assignments", connection);
		query.addStr ("asnkey",     null);
		query.addNum ("cnum",       confKey);
		query.addStr ("instructor", root.getChildText("instructor", caucus));
		query.addStr ("title",      root.getChildText("title", caucus));
		query.addStr ("start",      root.getChildText("start", caucus));
		query.addStr ("end",        root.getChildText("end", caucus));
		query.addStr ("due",        root.getChildText("due", caucus));
		
		Element textElement = root.getChild("text", caucus);
		String text = translateElementToText (textElement);
		query.addStr ("text", text);
		query.addStr ("texttype",   textElement.getAttributeValue("type"));
		
		return query.execute();
	}
	
	protected void copyAttachedFiles(String xmlFileDir, Element root, String caucusHomeDir, String targetDir) {
		Namespace caucus = root.getNamespace();
		
		String attachDir = root.getChildText("attachDir", caucus);
		if (attachDir == null)  return;
		
		File src = new File(xmlFileDir + "/" + attachDir);
		File dst = new File(caucusHomeDir + "/public_html/LIB/" + targetDir);
		System.out.println ("copying " + src + " to " + dst);
		FileUtils.copyDirContentsToDir (src, dst, false);
	}
	
	protected void addResponsesToItem(Map<String,Element> responses, String item, Element root, Map<String,String> responseTypes) {
		Namespace caucus = root.getNamespace();
		
		//executeSimpleQuery ("INSERT INTO resps (items_id, rnum) VALUES (" + item + ", -1)", connection);

		for (int rnum=0;  rnum < responses.size();  ++rnum) {
			Element response = (Element) responses.get(String.valueOf(rnum));
			if (response == null) continue;
			
			InsertQuery query = new InsertQuery("INSERT INTO resps", connection);
			query.addNum("items_id", item);
			query.addStr("rnum",     response.getChildText("rnum", caucus));
			query.addStr("userid",   response.getChildText("userid", caucus));
			query.addNum("bits",     response.getAttributeValue("bits"));
			query.addNum("deleted",  response.getAttributeValue("deleted"));
			query.addStr("time",     response.getChildText("time", caucus));
			
			query.addNumIfNotNull("version",   response.getChildText("version", caucus));
			query.addNumIfNotNull("versioned", response.getChildText("versioned", caucus));
			query.addNumIfNotNull("approved",  response.getChildText("approved", caucus));
			query.addStrIfNotNull("editid",    response.getChildText("editid", caucus));
			query.addStrIfNotNull("edittime",  response.getChildText("edittime", caucus));
			
			Element textElement = response.getChild("text", caucus);
			String text = translateElementToText (textElement);
			query.addStr("text", text);
			query.addNum("prop", (String) responseTypes.get(textElement.getAttributeValue("type")));
			query.execute();
		}
		
		//executeSimpleQuery ("DELETE FROM resps WHERE items_id=" + item + " AND rnum = -1", connection);
		
		String setLastResp = "UPDATE items SET lastresp=" + (responses.size()-1) + " WHERE id=" + item;
		MySqlUtils.executeUpdate (setLastResp, connection);
	}
	
	protected void addQuizToItem (String itemKey, Element root) {
		Namespace caucus = root.getNamespace();
		Element quiz = root.getChild("quiz", caucus);
		if (quiz == null)  return;
		
		String userid = MySqlUtils.executeSimpleQueryWithOneResult("SELECT userid FROM items WHERE id=" + itemKey, connection);
		InsertQuery query = new InsertQuery ("INSERT INTO quiz ", connection);
		query.addStr("qkey", null);
		query.addNum("onepage",    quiz.getAttributeValue("onepage"));
		query.addStr("seesummary", quiz.getAttributeValue("seesummary"));
		query.addNum("seeAnswers", quiz.getAttributeValue("seeAnswers"));
		query.addStr("userid", userid);
		Element visible0 = quiz.getChild("visible0", caucus);
		Element visible1 = quiz.getChild("visible1", caucus);
		query.addStr("visible0", visible0.getText());
		query.addStr("visible1", visible1.getText());
		String qkey = query.execute();
		
		String setItemQkey = "UPDATE items SET qkey=" + qkey + " WHERE id=" + itemKey;
		MySqlUtils.executeUpdate (setItemQkey, connection);
		
		addAllProblemsToQuiz(qkey, quiz, userid, caucus);
	}
	
	protected void addAllProblemsToQuiz (String qKey, Element quiz, String userid, Namespace caucus) {
		List<Element> problems = quiz.getChildren("quizproblem", caucus);
		for (Element problem: problems) {
			String pKey = addProblemToQuiz (qKey, problem, userid, caucus);
			connectProblemToQuiz(qKey, pKey);
			addAllAnswersToProblem (pKey, problem, caucus);
		}
	}
	
	protected void addAllAnswersToProblem (String pKey, Element problem, Namespace caucus) {
		List<Element> answers = problem.getChildren("quizanswer", caucus);
		for (Element answer: answers) {
			addAnswerToProblem(pKey, answer, caucus);
		}
	}
	
	protected void addAnswerToProblem (String pKey, Element answer, Namespace caucus) {
		InsertQuery query = new InsertQuery ("INSERT INTO quizanswers ", connection);
		query.addNum("akey", null);
		query.addNum("pkey", pKey);
		query.addStr("text1", answer.getChildText("text1", caucus));
		query.addStr("type",  answer.getChildText("type", caucus));
		query.addStr("text2", answer.getChildText("text2", caucus));
		query.addStr("answer", answer.getChildText("answer", caucus));
		query.addNum("sortorder", answer.getChildText("sortorder", caucus));
		query.addNum("points", answer.getChildText("points", caucus));
		query.execute();
	}
	
	protected String addProblemToQuiz (String qkey, Element problem, String userid, Namespace caucus) {
		InsertQuery query = new InsertQuery ("INSERT INTO quizproblems ", connection);
		query.addNum ("pkey", null);
		query.addNum ("floating", problem.getAttributeValue("floating", caucus));
		query.addStr ("owner", userid);
		query.addStr ("title", problem.getChildText("title", caucus));
		query.addStr ("text", problem.getChildText("text", caucus));
		query.addStr ("texttype", problem.getChildText("texttype", caucus));
		query.addNum ("totalpoints", problem.getChildText("points", caucus));
		
		//TODO category?
		
//         <caucus:quizanswer newrow="1">
//            <caucus:text1>Earth-like planets</caucus:text1>
//            <caucus:type>tf</caucus:type>
//            <caucus:text2></caucus:text2>
//            <caucus:answer>f</caucus:answer>
//            <caucus:sortorder>0</caucus:sortorder>
//            <caucus:points>2</caucus:points>
//         </caucus:quizanswer>
		
//		category 	varchar(120) 			NULL 	General category of problem (arbitrary text)
//		reference 	varchar(255) 			NULL 	

		String pKey = query.execute();
		return pKey;
	}
	
	protected void connectProblemToQuiz (String qKey, String pKey) {
		InsertQuery query = new InsertQuery ("INSERT INTO quizmap ", connection);
		query.addNum ("qkey", qKey);
		query.addNum ("pkey", pKey);
		query.execute();
	}
	
	protected static String translateElementToText (Element element) {
		String result = "";
		if (element == null)  return result;
		
		List tlist = element.getContent();
		for (Iterator t = tlist.iterator();   t.hasNext();) {
			Object x = t.next();
			if (x instanceof Text) result = result + ((Text) x).getText();
			if (x instanceof Element) {
				String macro = ((Element) x).getAttributeValue("macro");
				result = result + MiscUtils.null2Empty(macro);
			}
		}
		
		return result;
	}
	
	protected static Map<String,Element> getResponses (String item, Element root) {
		Namespace caucus = root.getNamespace();
		Map<String,Element> responses = new HashMap<String,Element>();
		
		List<Element> responseList = (List<Element>) root.getChildren("response", caucus);
      for (Iterator<Element> s = responseList.iterator();   s.hasNext();) {
         Element response = (Element) s.next();
         String rnum = response.getChildText("rnum", caucus);
         if (rnum != null) responses.put(rnum, response);
      }
      
      return responses;
	}
	
	protected void addItemToConference(String item, Element root, String confKey, Map<String,String> itemLabels) {
		Namespace caucus = root.getNamespace();
		String[] label = (root.getChildText("label", caucus) + ".0.0.0.0.0.0").split("\\.");
		
		String itemsParent = MiscUtils.null2Zero ((String) itemLabels.get(root.getAttributeValue("itemParent")));

		InsertQuery query = new InsertQuery("INSERT INTO conf_has_items", connection);
		query.addNum ("cnum",         confKey);
		query.addNum ("items_id",     item);
		query.addNum ("hidden",       root.getAttributeValue("hidden"));
		query.addNum ("retired",      root.getAttributeValue("retired"));
		query.addNum ("items_parent", itemsParent);
		for (int i=0;   i<6;   ++i) query.addNum ("item" + i, label[i]);
		
		query.execute();
	}
	
	protected static int parseInt (String str) {
		int result = 0;
		try { result = Integer.parseInt(str); }
		catch (Exception e) {
		}
		return result;
	}
	
	protected String createItem (Element root, Map<String,String> itemLabels) {
		Namespace caucus = root.getNamespace();
		
		InsertQuery query = new InsertQuery("INSERT INTO items", connection);
		query.addStr ("id",     null);
		query.addStr ("title",  root.getChildText("title",  caucus));
		query.addStr ("userid", root.getChildText("userid", caucus));
		query.addNum ("frozen", root.getAttributeValue("frozen"));
		query.addStrIfNotNull("type", root.getChildText("type", caucus));
	
		System.out.println ("query=" + query);
		
		String itemKey = query.execute();
		if (! itemKey.equals("")) {
			itemLabels.put(root.getChildText("label", caucus), itemKey);
			System.out.println ("itemkey=" + itemKey + " label=" + root.getChildText("label", caucus));
		}
		
		return itemKey;
	}
	
   private static Element loadXMLFile(String fileName) {
      File configFile = new File(fileName);
      if (!configFile.canRead()) return (Element) null;

      try {
         SAXBuilder builder = new SAXBuilder();
         Document doc = builder.build(configFile);
         Element root = doc.getRootElement();
         return root;
      } catch (Exception e) {
         e.printStackTrace();
         return (Element) null;
      }
   }

	
	protected static Connection getMysqlConnectionFromCaucusConnectString (String connectString) {
		Connection connection = null;
		
		//--- E.g. connectString="DB=caucus_c53;UID=c53;PWD=lookfar"
		String userName = pluckFieldFromText ("UID=",  connectString);
		String password = pluckFieldFromText ("PWD=",  connectString);
		String port     = pluckFieldFromText ("PORT=", connectString);
		if (port == null  ||  port.equals(""))  port = "3306";
		String url = "jdbc:mysql://127.0.0.1:" + port + "/" + pluckFieldFromText ("DB=", connectString);
		
		connection = MySqlUtils.makeMySqlConnectionFromJdbcUrl(url, userName, password);
      return connection;
	}
	
	protected static String pluckFieldFromText(String fieldName, String text) {
		text = text + ";";
		int fieldPos = text.indexOf(fieldName);
		if (fieldPos < 0)  return "";
		int stopPos = text.indexOf(";", fieldPos);
		if (stopPos < 0)   return "";
		return text.substring(fieldPos + fieldName.length(), stopPos);
	}


}
