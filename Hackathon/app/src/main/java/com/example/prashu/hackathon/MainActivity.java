package com.example.prashu.hackathon;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;




public class MainActivity extends AppCompatActivity {

    private EditText Name;
    private EditText Password;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button in;
        Name = findViewById(R.id.uname);
        Password = findViewById(R.id.upass);
        in = findViewById(R.id.login);

        in.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Validate(Name.getText().toString(),Password.getText().toString());
            }
        });
    }

    private void Validate(String userName , String userPassword){
        if ( (userName.equals("admin")) && (userPassword.equals("hello")) ) {
            Intent intent = new Intent(MainActivity.this, SecondActivity.class);
            startActivity(intent);
        }
    }
}
