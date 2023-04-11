package com.example.iot_mosquitto

import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper

class DataBase(context: Context) : SQLiteOpenHelper(context, DATABASE_NAME, null, DATABASE_VERSION) {

    companion object {
        private const val DATABASE_VERSION = 1
        private const val DATABASE_NAME = "dataBase.db"
    }

    override fun onCreate(db: SQLiteDatabase) {
        // Crea la tabla si no existe
        db.execSQL("CREATE TABLE IF NOT EXISTS serverMQTT (id INTEGER PRIMARY KEY AUTOINCREMENT, ip TEXT)")
    }

    override fun onUpgrade(db: SQLiteDatabase, oldVersion: Int, newVersion: Int) {
        // Actualiza la tabla si es necesario
    }
}
