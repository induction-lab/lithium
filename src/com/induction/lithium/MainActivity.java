package com.induction.lithium;

/* Java helper for native application */

import android.app.NativeActivity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowManager.LayoutParams;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

public class MainActivity extends NativeActivity {
	public static final String APP_TITLE = "lithium";
    MainActivity activity;
    PopupWindow popupWindow;
    TextView label;
    protected void onResume() {
        super.onResume();
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }
    protected void onDestroy() {
        if (popupWindow != null) {
            Log.i(APP_TITLE, "[java] Dismiss base UI from Java code.");
            popupWindow.dismiss();
        }
        super.onDestroy();
    }
    public void showUI() {
		Log.i(APP_TITLE, "[java] Show base UI from Java code.");
        if (popupWindow != null) return;
        activity = this;
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                LayoutInflater layoutInflater = (LayoutInflater)getBaseContext().getSystemService(LAYOUT_INFLATER_SERVICE);
                View popupView = layoutInflater.inflate(R.layout.fps, null);
                popupWindow = new PopupWindow(popupView, LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
                LinearLayout mainLayout = new LinearLayout(activity);
                MarginLayoutParams params = new MarginLayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
                params.setMargins(0, 0, 0, 0);
                activity.setContentView(mainLayout, params);
                // Show our UI over NativeActivity window
                popupWindow.showAtLocation(mainLayout, Gravity.CENTER | Gravity.TOP, 0, 35);
                popupWindow.update();
                label = (TextView)popupView.findViewById(R.id.textViewFPS);
            }
        });
    }
    public void updateFPS(final float fFPS) {
        if (label == null) return;
        activity = this;
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                label.setText(String.format("%2.2f FPS", fFPS));
            }
        });
    }
}