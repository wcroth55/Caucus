import java.io.*;

public class FileUtils {

	/**
	 * Create a directory in the operating system's "temporary" area.
	 * While in theory the OS will eventually delete this directory and
	 * eveything in it, it's still a good idea to delete it yourself
	 * when you know you are done with it.  (See recursiveDelete().)
	 * 
	 * @param prefix Used as the initial part of the temporary directory's 
	 * name, Java will add some other pseudo-random characters.
	 * 
	 * @return Created directory, or null on any error.
	 */
	public static File createTemporaryDirectory(String prefix) {
		File tempDir;
		try {
			tempDir = File.createTempFile(prefix, "");
			if (!tempDir.delete())
				return null;
			if (!tempDir.mkdir())
				return null;
		} catch (Exception e) {
			tempDir = null;
		}
		return tempDir;
	}

	/**
	 * Delete a file or directory, and (recursively) everything underneath it.
	 *
	 * @param path
	 * @return true on complete success, false if any file or directory could not be deleted.
	 */
	public static boolean recursiveDelete(File path) {
		if (!path.exists())
			return false;
		boolean success = true;

		for (File f : path.listFiles()) {
			if (f.isDirectory())
				success = success && recursiveDelete(f);
			else
				success = success && safelyDelete(f);
		}
		return (success && safelyDelete(path));
	}

	protected static boolean safelyDelete(File path) {
		boolean success = false;
		try {
			success = path.delete();
		} catch (Exception e) {
		}
		return success;
	}

	/**
	 * Open a text file for reading.
	 * @param fileName full pathname of text file to read.
	 * @return BufferedReader ready for reading, or null on error.
	 */
	public static BufferedReader makeReaderFromFilename(String fileName) {
		BufferedReader in = null;
		try {
			File file = new File(fileName);
			InputStream stream = new FileInputStream(file);
			InputStreamReader reader = new InputStreamReader(stream);
			in = new BufferedReader(reader);
		} catch (Exception e) {
		}
		return in;
	}
	
	/**
	 * Open a text file for writing.
	 * @param filename
	 * @return PrintWriter ready for writing, or null on error.
	 */
	public static PrintWriter makeWriterFromFilename(String filename) {
		try {
			PrintWriter pw = new PrintWriter (new FileWriter (new File(filename)));
			return pw;
		}
		catch (Exception e) {
			return null;
		}
	}

	/**
	 * "Safely" close a BufferedReader.  Handles null, and swallows any exceptions.
	 * @param in
	 */
	public static void safelyClose(BufferedReader in) {
		if (in == null)
			return;
		try {
			in.close();
		} catch (Exception e) {
		}
	}
	
	/**
	 * "Safely" close a PrintWriter.  Handles null, and swallows any exceptions.
	 * 
	 */
	public static void safelyClose (PrintWriter p) {
		if (p == null)  return;
		try {
			p.close();
		} catch (Exception e) {
		}
	}

	/**
	 * Read a line "safely" (w/o throwing exceptions) from a BufferedReader.
	 * @param in
	 * @return text of line in String, or null on error or EOF.
	 */
	public static String safelyReadLine(BufferedReader in) {
		String line = null;
		try {
			line = in.readLine();
		} catch (Exception e) {
		}
		return line;
	}

	/**
	 * Are the two given text files identical?
	 * @param fileA
	 * @param fileB
	 * @return True if the files are identical, false otherwise, and
	 * false if either file does not exist or cannot be read.
	 */
	public static boolean areTextFilesIdentical (String fileA, String fileB) {
		System.out.println (fileA);
		System.out.println (fileB);
		BufferedReader a = makeReaderFromFilename(fileA);
		BufferedReader b = makeReaderFromFilename(fileB);
		boolean identical = false;
		
		if (a!=null  &&  b!=null) {
			identical = true;
			String lineA, lineB;
			while (identical) {
				lineA = safelyReadLine (a);
				lineB = safelyReadLine (b);
				System.out.println (lineA);
				if (lineA == null  &&  lineB == null)  break;
				if (lineA == null  ||  lineB == null || ! lineA.equals(lineB)) identical = false;
			}
		}
		
		safelyClose(a);
		safelyClose(b);
		return identical;
	}
}
