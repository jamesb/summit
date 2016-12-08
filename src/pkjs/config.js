module.exports = [
  {
    "type": "heading",
    "defaultValue": "Summit"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "select",
        "messageKey": "CURRENCY_SYMBOL",
        "label": "Currency Symbol",
        "defaultValue": "$",
        "options": [
          {
            "label": "$ (Dollar)",
            "value": "$"
          },
          {
            "label": "€ (Euro)",
            "value": "€"
          },
          {
            "label": "£ (Pound)",
            "value": "£"
          },
          {
            "label": "¥ (Yen / Yuan)",
            "value": "¥"
          },
          {
            "label": "¤ (Generic)",
            "value": "¤"
          },
          {
            "label": "(No Symbol)",
            "value": ""
          }
        ]
      },
      {
        "type": "slider",
        "messageKey": "DEFAULT_KILO_SALARY",
        "defaultValue": 50,
        "label": "Default Annual Wage (in thousands)",
        "description": "Average salary to use for meeting attendees; eg. 50 = $50,000",
        "min": 10,
        "max": 200,
        "step": 10
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
