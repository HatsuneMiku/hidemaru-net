import java.io.File;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ResourceBundle;

public class Hm {

	static int count;
	// ���C�u���������[�h
	static { System.loadLibrary("hmJavaVM"); }
	
	protected static native double GetVersion();

	protected static native void DebugInfo(String message);

	protected static native int EvalMacro(String message);

	protected static native String GetMacroVarObj(String name);
	protected static native int SetMacroVarNum(String name, long value);
	protected static native int SetMacroVarStr(String name, String value);

	protected static native String GetTotalText();
	protected static native int SetTotalText(String text);

	protected static native String GetSelectedText();
	protected static native int SetSelectedText(String text);

	protected static native String GetLineText();
	protected static native int SetLineText(String text);

	protected static native String GetCursorPos();
	protected static native String GetCursorPosFromMousePos();

	public static double getVersion() {
		return GetVersion();
	}

	public static void DebugInfo(Object text, Object... args) {
		String begin = text.toString();

		for( Object arg : args ) {
			begin += arg.toString();
		}
		DebugInfo(begin);
	}

	public static class Edit {
		public static String getTotalText() {
			return GetTotalText();
		}
		public static boolean setTotalText(Object value) {
			int ret = SetTotalText(value.toString());
			if (ret == 0) {
				throw new java.lang.RuntimeException("Can't Edit Hidemaru TotalText");
			}
			return true;
		}
		public static String getSelectedText() {
			return GetSelectedText();
		}
		public static boolean setSelectedText(Object value) {
			int ret = SetSelectedText(value.toString());
			if (ret == 0) {
				throw new java.lang.RuntimeException("Can't Edit Hidemaru SelectedText");
			}
			return true;
		}
		public static String getLineText() {
			return GetLineText();
		}
		public static boolean setLineText(Object value) {
			int ret = SetLineText(value.toString());
			if (ret == 0) {
				throw new java.lang.RuntimeException("Can't Edit Hidemaru LineText");
			}
			return true;
		}

		public static class CursorPos {
			private int m_lineno;
			private int m_column;

			public CursorPos(int lineno, int column) {
				this.m_lineno = lineno;
				this.m_column = column;
			}

			public int getLineNo() {
				return m_lineno;
			}
			public int getColumn() {
				return m_column;
			}
		}

		public static CursorPos getCursorPos() {
			String strPos = GetCursorPos();
			String[] lineno_and_column = strPos.split(",", 2);
			int lineno = Integer.parseInt(lineno_and_column[0]);
			int column = Integer.parseInt(lineno_and_column[1]);
			CursorPos pos = new CursorPos(lineno, column);
			return pos;
		}

		public static CursorPos getCursorPosFromMousePos() {
			String strPos = GetCursorPosFromMousePos();
			String[] lineno_and_column = strPos.split(",", 2);
			int lineno = Integer.parseInt(lineno_and_column[0]);
			int column = Integer.parseInt(lineno_and_column[1]);
			CursorPos pos = new CursorPos(lineno, column);
			return pos;
		}

	}

	public static class Macro {
		public static boolean Eval(Object expression) {
			int result = EvalMacro(expression.toString());
			if (result==0) {
				throw new java.lang.RuntimeException("Hidemaru Macro Expression Exception");
			}
			return true;
		}

		public static Object getVar(String symbol) {
			String result = GetMacroVarObj(symbol);
			if (result.contains("HmJavaVMType_Int32<<>>")) {
				result = result.replace("HmJavaVMType_Int32<<>>", "");
				int num = Integer.parseInt(result);
				return num;
			} else if (result.contains("HmJavaVMType_Int64<<>>")) {
				result = result.replace("HmJavaVMType_Int64<<>>", "");
				long num = Long.parseLong(result);
				return num;
			} else if (result.contains("HmJavaVMType_String<<>>")) {
				result = result.replace("HmJavaVMType_String<<>>", "");
				return result;
			}

			throw new java.lang.IllegalArgumentException("Can't find Hidemaru Macro Simbol " + symbol);
		}

		public static boolean setVar(String symbol, Object value) {
			int ret = 0;
			if (symbol.startsWith("#")) {
				String strValue = value.toString();
				long num = new Long(strValue).longValue();
				ret = SetMacroVarNum(symbol, num);
			}
			else if (symbol.startsWith("$")) {
				String str = value.toString();
				ret = SetMacroVarStr(symbol, str);
			}

			if (ret != 0) {
				return true;
			} else {
				throw new java.lang.IllegalArgumentException("Can't assign Hidemaru Macro Simbol " + symbol);
			}
		}
	}

	/**
	 * �}�N�������݂���p�X�̒ǉ��B
	 * ���̃p�X��lib�T�u�f�B���N�g���̒ǉ��B
	 */
	public static void _AddClassPath(String[] _dummy){
		String path1 = (String)Hm.Macro.getVar("currentmacrodirectory");
		_AddClassPath(path1);
		String path2 = (String)Hm.Macro.getVar("currentmacrodirectory") + "\\lib";
		_AddClassPath(path2);
	}

	/**
	 * ClassPath�̒ǉ�
	 * @param path �ǉ�����PATH
	 */
    private static void _AddClassPath(String path) {
		URLClassLoader loader = (URLClassLoader)ClassLoader.getSystemClassLoader();
		try {
			URL u = new File(path).toURI().toURL();
			Method m = URLClassLoader.class.getDeclaredMethod("addURL", new Class[]{ URL.class });
			m.setAccessible(true);
			m.invoke(loader, new Object[]{u});
			Hm.DebugInfo("ClassPath�Ɂu" + path + "�v��ǉ����܂���");
		} catch (Exception e){
			throw new RuntimeException("ClassPath�̒ǉ��Ɏ��s���܂����B(" + path + ")", e);
		}
	}

	// static�R���X�g���N�^���������s�����邽�߂ɕK�v�B
	public static void _Init(String[] _dummy) {
	}
}

