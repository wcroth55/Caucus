import java.io.*;

public class CaucusMacroDoc {
	private static final int IN_CODE = 0;
	private static final int IN_MACRO = 1;

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length < 2) {
			System.err.println ("Usage: CaucusMacroDoc inputFile outputFile");
			System.exit(1);
		}
		BufferedReader in = FileUtils.makeReaderFromFilename (args[0]);
		exitOnNull (in, "Cannot open file " + args[0]);
		
		PrintWriter out = FileUtils.makeWriterFromFilename (args[1]);
		exitOnNull (out, "Cannot open file " + args[1]);
		writeHtmlPrologue(out);
		
		boolean isMacroStart = false;
		boolean isComment = false;
		int state = IN_CODE;
		String line;
		while ( (line = FileUtils.safelyReadLine(in)) != null) {
			line = line.trim() + " ";
			
			if (line.startsWith("#@")) {
				out.println (line.substring(2));
				continue;
			}
			
			isMacroStart = false;
			isComment = (line.charAt(0) == '#');
			if (isComment) {
				String macroLeadIn = line.substring(1).trim() + "#";
				isMacroStart = (macroLeadIn.charAt(0) == '%');
			}
			
			if (state == IN_CODE) {
				if (isMacroStart) {
					state = IN_MACRO;
					startMacroDef(out, line.substring(1));
				}
			}
			
			else if (state == IN_MACRO) {
				if (isMacroStart) {
					startMacroDef (out, line.substring(1));
				}
				else if (isComment) {
					String explanation = emphasizeSingleQuotedWords(line.substring(1));
					explanation = explanation.replaceAll("\\. ", ".&nbsp; ");
					out.println (explanation);
				}
				else {
					endMacroDef(out);
					state = IN_CODE;
				}
			}
			
		}
		FileUtils.safelyClose(in);
		
		out.println("</body>");
		out.println("</html>");
		FileUtils.safelyClose(out);
	}
	
	private static void endMacroDef(PrintWriter out) {
		out.println ("</dd>");
		out.println ("</dl>");
	}

	private static void writeHtmlPrologue(PrintWriter out) {
		out.println ("<html>");
		out.println ("<head>");
		out.println ("<title>Caucus Macros</title>");
		out.println ("<style type='text/css'>");
		out.println ("   body, td { font-family: arial;  font-size: 80%; }");
		out.println ("   LI { margin-top: .4em; }");
		out.println ("   pre { font-size: 120%; }");
		out.println ("   .arg { font-style: italic; }");
		out.println ("</style>");
		out.println ("</head>");

		out.println ("<body>");
		out.println ("<h2>Caucus Macros</h2>");
		//out.println ("<dl>");
	}
	
	private static String emphasizeSingleQuotedWords (String x) {
		x = x.replaceAll("'[A-Za-z_]+'", "<span class=\"arg\">$0</span>");
		x = x.replaceAll("'</span>", "</span>");
		x = x.replaceAll("arg\">'", "arg\">");
		return x;
	}

	private static void startMacroDef (PrintWriter out, String text) {
		out.println ("<p/><dl>");
		String name = text.replaceAll("^ *%", "");
		name = name.replaceAll("\\(.*$", "");
		text = text.replaceAll("\\{([A-Za-z_]+)\\}", "<a href='#$1'>$1</a>");
		if (text.indexOf("(") < 0) text = text.replaceFirst("%", "");
		out.println ("<dt><a name='" + name + "'>" + text + "</a></dt>");
		out.println ("<dd>");
	}
	
	public static void exitOnNull (Object x, String message) {
		if (x == null) {
			System.err.println (message);
			System.exit(1);
		}
	}

}
