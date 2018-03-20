package com.example.prashu.hackathon;

        import android.app.AlertDialog;
        import android.content.DialogInterface;
        import android.content.Intent;
        import android.support.v7.app.AppCompatActivity;
        import android.os.Bundle;
        import android.view.Menu;
        import android.view.SurfaceHolder;
        import android.view.View;
        import android.view.Window;
        import android.view.WindowManager;
        import android.widget.Button;
        import net.majorkernelpanic.streaming.Session;
        import net.majorkernelpanic.streaming.SessionBuilder;
        import net.majorkernelpanic.streaming.audio.AudioQuality;
        import net.majorkernelpanic.streaming.rtsp.RtspClient;
        import java.util.regex.Matcher;
        import java.util.regex.Pattern;

public class SecondActivity extends AppCompatActivity implements RtspClient.Callback,Session.Callback,SurfaceHolder.Callback
{
    public final static String TAG = MainActivity.class.getSimpleName();

    private Session mSession;
    private static RtspClient myClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_second);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);



        Button Facebook = findViewById(R.id.fb);
        Button Youtube = findViewById(R.id.yt);
        Button Instagram = findViewById(R.id.insta);
        Button Twitter = findViewById(R.id.tweet);

        initialize_rtsp();

        Facebook.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), webview.class);
                intent.putExtra("url", "http://www.facebook.com");
                startActivity(intent);
            }
        });

        Youtube.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), webview.class);
                intent.putExtra("url", "http://www.youtube.com");
                startActivity(intent);
            }
        });

        Instagram.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), SecondActivity.class);
                intent.putExtra("url", "http://www.instagram.com");
                startActivity(intent);
            }
        });

        Twitter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), SecondActivity.class);
                intent.putExtra("url", "http://www.twitter.com");
                startActivity(intent);
            }
        });

        /*
        if(runtime_permissions() == true)
        {
            startService(new Intent(this, GPS_Service.class));
        }
        else
        {
            String provider = Settings.Secure.getString(getContentResolver(), Settings.Secure.LOCATION_MODE);

            if(!provider.contains("gps")) { //if gps is disabled
                final Intent poke = new Intent();
                poke.setClassName("com.android.settings", "com.android.settings.widget.SettingsAppWidgetProvider");
                poke.addCategory(Intent.CATEGORY_ALTERNATIVE);
                poke.setData(Uri.parse("3"));
                sendBroadcast(poke);
            }
        }*/
    }
/*
    @Override
    protected void onResume() {
        super.onResume();
        startService(new Intent(this, GPS_Service.class));
    }

    @Override
    protected void onPause() {
        super.onPause();
        stopService(new Intent(this, GPS_Service.class));
    }

    private boolean runtime_permissions(){
        String permission = Manifest.permission.ACCESS_FINE_LOCATION;
        int res = this.checkCallingOrSelfPermission(permission);
        return (res == PackageManager.PERMISSION_GRANTED);
    }
    */

    public void initialize_rtsp(){
        mSession = SessionBuilder.getInstance().setContext(this)
                .setContext(getApplicationContext())
                .setAudioEncoder(SessionBuilder.AUDIO_NONE)
                .setAudioQuality(new AudioQuality(8000,16000))
                .setVideoEncoder(SessionBuilder.VIDEO_H264)
                .setCallback(this).build();

        myClient = new RtspClient();
        myClient.setSession(mSession);
        myClient.setCallback(this);

        String ip,port,path;
        Pattern uri = Pattern.compile("rtsp://(.+):(\\d+)/(.+)");
        Matcher m = uri.matcher(StreamInformation.STREAM_URL);
        m.find();
        ip = m.group(1);
        port = m.group(2);
        path = m.group(3);

        myClient.setCredentials(StreamInformation.PUBLISHER_USERNAME,StreamInformation.PUBLISHER_PASSWORD);
        myClient.setServerAddress(ip, Integer.parseInt(port));
        myClient.setStreamPath("/" + path);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return super.onCreateOptionsMenu(menu);
    }

    private void toggleStreaming() {
        if (!myClient.isStreaming()) {
            // Start camera preview
            mSession.startPreview();

            // Start video stream
            myClient.startStream();
        } else {
            // already streaming, stop streaming
            // stop camera preview
            mSession.stopPreview();

            // stop streaming
            myClient.stopStream();
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        toggleStreaming();
    }

    @Override
    protected void onPause() {
        super.onPause();
        toggleStreaming();
    }

    @Override
    public void onBitrateUpdate(long bitrate) {

    }

    @Override
    public void onSessionError(int reason, int streamType, Exception e) {

        switch (reason)
        {
            case Session.ERROR_CAMERA_ALREADY_IN_USE:
                break;
            case Session.ERROR_CAMERA_HAS_NO_FLASH:
                break;
            case Session.ERROR_CONFIGURATION_NOT_SUPPORTED:
                break;
            case Session.ERROR_INVALID_SURFACE:
                break;
            case Session.ERROR_STORAGE_NOT_READY:
                break;
            case Session.ERROR_OTHER:
                break;
        }
/*
        if (e!=null)
        {
            alertError(e.getMessage());
            e.printStackTrace();
        }
        */
    }
/*
    private void alertError(final String msg) {
        final String error = (msg == null) ? "Unknown error: " : msg;
        AlertDialog.Builder builder = new AlertDialog.Builder(SecondActivity.this);
        builder.setMessage(error).setPositiveButton("Ok",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                    }
                });
        AlertDialog dialog = builder.create();
        dialog.show();
    }
*/
    @Override
    public void onPreviewStarted() {

    }

    @Override
    public void onRtspUpdate(int message, Exception exception) {

        switch (message)
        {
            case RtspClient.ERROR_CONNECTION_FAILED:
            case RtspClient.ERROR_WRONG_CREDENTIALS:
               // alertError(exception.getMessage());
              //  exception.printStackTrace();
                break;
        }
    }

    @Override
    public void onSessionConfigured() {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void onSessionStarted() {

    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void onSessionStopped() {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        myClient.release();
        mSession.release();
    }
}