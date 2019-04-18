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
		Log.i(APP_TITLE, "Set immersive mode.");
        // Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 11 && SDK_INT < 14) {
            getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
        } else if(SDK_INT >= 14 && SDK_INT < 19) {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LOW_PROFILE);
        } else if(SDK_INT >= 19) {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
        }
    }
    public void showUI() {
		Log.i(APP_TITLE, "Show base UI from Java code.");
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
                popupWindow.showAtLocation(mainLayout, Gravity.CENTER | Gravity.TOP, 0, 50);
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