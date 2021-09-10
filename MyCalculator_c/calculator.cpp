#include "calculator.h"
#include <QDebug>
#include <QGridLayout>
#include <QtMath>

Calculator::Calculator() {
  setFixedSize(width(), height());
  m_display_up = new QLineEdit();
  m_display_down = new QLineEdit();
  m_sign = new QLabel();
  QGridLayout *mainLayout = new QGridLayout;

  mainLayout->setSizeConstraint(QLayout::SetFixedSize);

  m_display_down->setText("0");

  m_display_up->setReadOnly(true);
  m_display_down->setReadOnly(true);

  m_sign->setAlignment(Qt::AlignRight);
  m_display_up->setAlignment(Qt::AlignRight);
  m_display_down->setAlignment(Qt::AlignRight);

  m_display_up->setMaxLength(15);
  m_display_down->setMaxLength(15);

  QFont font = m_display_up->font();
  font.setPointSize(font.pointSize() + 6);
  m_display_up->setFont(font);
  m_display_down->setFont(font);
  m_sign->setFont(font);

  for (int i = 0; i < 10; i++)
    m_digitMyButton[i] =
        createMyButton(QString::number(i), SLOT(digitClicked()));

  MyButton *pointMyButton = createMyButton(".", SLOT(pointClicked()));
  MyButton *changeSignMyButton =
      createMyButton(m_change_sign, SLOT(changeSignClicked()));
  MyButton *backspaceMyButton =
      createMyButton("Backspace", SLOT(backspaceClicked()));
  MyButton *clearMyButton = createMyButton("Clear", SLOT(clear()));
  MyButton *clearAllMyButton = createMyButton("Clear All", SLOT(clearAll()));
  MyButton *clearMemoryMyButton = createMyButton("MC", SLOT(clearMemory()));
  MyButton *readMemoryMyButton = createMyButton("MR", SLOT(readMemory()));
  MyButton *setMemoryMyButoon = createMyButton("M+", SLOT(addToMemory()));
  MyButton *addToMemoryButton = createMyButton("M-", SLOT(minToMemory()));
  MyButton *divisionMyButton =
      createMyButton(m_division_sign, SLOT(doubleOperatorClicked()));
  MyButton *timesMyButton =
      createMyButton(m_times_sign, SLOT(doubleOperatorClicked()));
  MyButton *minusMyButton =
      createMyButton(m_minus_sign, SLOT(doubleOperatorClicked()));
  MyButton *plusMyButton =
      createMyButton(m_plus_sign, SLOT(doubleOperatorClicked()));
  MyButton *squareRootMyButton =
      createMyButton(m_squareRoot_sign, SLOT(unaryOperatorClicked()));
  MyButton *powerMyButton =
      createMyButton(m_power_sign, SLOT(unaryOperatorClicked()));
  MyButton *reciprocalMyButton =
      createMyButton(m_reciprocal_sign, SLOT(unaryOperatorClicked()));
  MyButton *equalMyButton = createMyButton("=", SLOT(equalClicked()));

  mainLayout->addWidget(m_display_up, 0, 0, 1, 6);
  mainLayout->addWidget(m_sign, 1, 5, 1, 1);
  mainLayout->addWidget(m_display_down, 2, 0, 1, 6);

  mainLayout->addWidget(backspaceMyButton, 3, 0, 1, 2);
  mainLayout->addWidget(clearMyButton, 3, 2, 1, 2);
  mainLayout->addWidget(clearAllMyButton, 3, 4, 1, 2);

  mainLayout->addWidget(clearMemoryMyButton, 4, 0);
  mainLayout->addWidget(readMemoryMyButton, 5, 0);
  mainLayout->addWidget(setMemoryMyButoon, 6, 0);
  mainLayout->addWidget(addToMemoryButton, 7, 0);

  for (int i = 1; i < 10; ++i) {
    int row = ((9 - i) / 3) + 4;
    int column = ((i - 1) % 3) + 1;
    mainLayout->addWidget(m_digitMyButton[i], row, column);
  }

  mainLayout->addWidget(m_digitMyButton[0], 7, 1);
  mainLayout->addWidget(pointMyButton, 7, 2);
  mainLayout->addWidget(changeSignMyButton, 7, 3);

  mainLayout->addWidget(divisionMyButton, 4, 4);
  mainLayout->addWidget(timesMyButton, 5, 4);
  mainLayout->addWidget(minusMyButton, 6, 4);
  mainLayout->addWidget(plusMyButton, 7, 4);

  mainLayout->addWidget(squareRootMyButton, 4, 5);
  mainLayout->addWidget(powerMyButton, 5, 5);
  mainLayout->addWidget(reciprocalMyButton, 6, 5);
  mainLayout->addWidget(equalMyButton, 7, 5);

  setLayout(mainLayout);

  setWindowTitle("Калькулятор");
}

MyButton *Calculator::createMyButton(const QString &text, const char *member) {
  MyButton *btn = new MyButton(text);
  connect(btn, SIGNAL(clicked()), this, member);
  return btn;
}

void Calculator::digitClicked() {
  MyButton *btn = (MyButton *)sender();
  int digit = btn->text().toUInt();

  if (m_display_down->text() == "Помилка")
    return;
  else if (m_display_down->text() == "0") {
    m_display_down->clear();
    m_display_up->clear();
  }
  m_display_down->setText(m_display_down->text() + QString::number(digit));
}

void Calculator::unaryOperatorClicked() {
  MyButton *btn = (MyButton *)sender();
  QString operation = btn->text();

  double operand = m_display_down->text().toDouble();
  double result = 0.0;

  if (operation == m_squareRoot_sign) {
    if (operand < 0.0) {
      abortOperation();
      return;
    }
    result = std::sqrt(operand);
  } else if (operation == m_power_sign) {
    result = std::pow(operand, 2);
  } else if (operation == m_reciprocal_sign) {
    if (operand == 0.0) {
      abortOperation();
      return;
    }

    result = 1.0 / operand;
  }
  m_display_down->setText(QString::number(result));
}

void Calculator::doubleOperatorClicked() {
  MyButton *btn = (MyButton *)sender();
  QString operation = btn->text();

  double operand = m_display_down->text().toDouble();

  if (m_display_down->text() == "0")
    return;

  m_sign->setText(operation);

  if (m_display_down->text() == "")
    return;
  m_display_down->clear();

  if (!m_pending_operation.isEmpty()) {
    if (!calculate(operand, m_pending_operation)) {
      abortOperation();
      return;
    }
  } else {
    m_display_up->setText(QString::number(operand));
  }

  m_pending_operation = operation;
}

void Calculator::equalClicked() {
  double operand = m_display_down->text().toDouble();
  if (!m_pending_operation.isEmpty()) {
    if (!calculate(operand, m_pending_operation)) {
      abortOperation();
      return;
    }
    m_pending_operation.clear();
  }
  m_display_down->setText(m_display_up->text());
  m_display_up->clear();
  m_sign->clear();
}

void Calculator::pointClicked() {
  if (!m_display_down->text().contains('.')) {
    m_display_down->setText(m_display_down->text() + '.');
  }
}

void Calculator::changeSignClicked() {
  QString text = m_display_down->text();
  double val = text.toDouble();

  if (val < 0.0)
    text.remove(0, 1);
  else if (val > 0.0)
    text.prepend("-");
  m_display_down->setText(text);
}
void Calculator::backspaceClicked() {
  QString text = m_display_down->text();
  text.chop(1);
  m_display_down->setText(text);
}

void Calculator::clear() { m_display_down->clear(); }

void Calculator::clearAll() {
  m_display_down->setText("0");
  m_display_up->clear();
  m_sign->clear();
}

void Calculator::clearMemory() { m_sum_in_Memory = 0.0; }

void Calculator::readMemory() {
  m_display_up->clear();
  m_sign->clear();
  m_display_down->setText(QString::number(m_sum_in_Memory));
}

void Calculator::addToMemory() {
  equalClicked();
  m_sum_in_Memory += m_display_down->text().toDouble();
}

void Calculator::minToMemory() {
  equalClicked();
  m_sum_in_Memory -= m_display_down->text().toDouble();
}

void Calculator::abortOperation() { m_display_down->setText("Помилка"); }

bool Calculator::calculate(double operand, const QString &operation) {
  double temp_total = m_display_up->text().toDouble();

  if (operation == m_plus_sign) {
    temp_total += operand;
  } else if (operation == m_minus_sign) {
    temp_total -= operand;
  } else if (operation == m_times_sign) {
    temp_total *= operand;
  } else if (operation == m_division_sign) {
    if (operand == 0.0)
      return false;
    temp_total /= operand;
  }

  m_display_up->setText(QString::number(temp_total));
  return true;
}
