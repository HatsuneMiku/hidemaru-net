import java.awt.*;

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

		int a = 1 / 0;
		Hm.count++;
		
		String vv = hm.MacroVarGetObj("filename2");
		hm.DebugInfo("������");
		hm.DebugInfo(Hm.count + "");
		hm.MacroVarSetNum("#test1" , 100 );
		hm.MacroVarSetStr("$test1" , "500aa����������" );
		System.out.println(vv);
		WindowTest windowText=new WindowTest();
	}
}

//
// WindowTest �N���X��, Window�̕\�����s���܂��B
//
class WindowTest{

   //�R���X�g���N�^
  WindowTest(){

     //Window�����
    Frame frame;//Frame �^�ϐ���錾
    frame=new Frame("Window Test");//Frame�^�ϐ��ɏ����l���Z�b�g
    frame.setSize(240, 240);//�T�C�Y���w�肷
    frame.setVisible(true);//��������

    //WIndow�ɕ\������B
    Label label;//�\���p�̕��i�i�����ł̓��x���j��錾
    label=new Label("Hellow Window");//�\���p���i�ɏ����l���Z�b�g
    frame.add(label);//�\���p�̕��i��Window�ɒǉ�
  }
 }