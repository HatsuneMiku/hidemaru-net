
public class Hm {
	static int count;
	// ���C�u���������[�h
	static {System.loadLibrary("hmJavaVM");}
	
	public native void DebugInfo(String message);

	public native String MacroVarGetObj(String name);

	public native boolean MacroVarSetNum(String name, long value);

	public native boolean MacroVarSetStr(String name, String value);

	public static void main(String[] args) {
		Hm hm = new Hm();

		Hm.count++;
		
		String vv = hm.MacroVarGetObj("filename2");
		hm.DebugInfo("������");
		hm.DebugInfo(Hm.count + "");
		hm.MacroVarSetNum("#test1" , 100 );
		hm.MacroVarSetStr("$test1" , "500aa����������" );
		System.out.println(vv);
	}
}